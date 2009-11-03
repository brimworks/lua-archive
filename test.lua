print "1..6"

local tap     = require("tap")
local archive = require("archive")
local ok      = tap.ok
local io      = require("io")

function main()
   test_missing_reader()
   test_missing_writer()
   test_basic()
end

function test_missing_writer()
   local success, err = pcall(function ()
                                 archive.write {}
                              end)
   ok(not success, "archive.write should fail with missing argument")
   ok(string.match(err, "^.*MissingArgument"),
      "got MissingArgument error (" .. err .. ")")
end

function test_missing_reader()
   local success, err = pcall(function ()
                                 archive.read {}
                              end)
   ok(not success, "archive.read should fail with missing argument")
   ok(string.match(err, "^.*MissingArgument"),
      "got MissingArgument error (" .. err .. ")")
end

function test_basic()
   local this_file = string.sub(debug.getinfo(1,'S').source, 2)

   local bytes_written = 0
   local tmpfh = io.tmpfile()--assert(io.open("/tmp/test.tar", "w+b"))
   local function writer(ar, str)
      if ( nil == str ) then
         tmpfh:flush()
      else
         tmpfh:write(str)
         bytes_written = bytes_written + #str
         return #str
      end
   end

   -- make it as small as possible by reducing the bytes per block and
   -- setting the compression level to 9.
   local ar = archive.write {
      writer = writer,
      compression = "gzip",
      bytes_in_last_block = 1,
      bytes_per_block = 100,
      options = "compression-level=9",
   }
   local fh = assert(io.open(this_file, "rb"))

   -- Test passing in a file name:
   ar:header(archive.entry(this_file))
   while ( true ) do
      local buff = fh:read(10) -- Test doing a lot of reads.
      if ( nil == buff ) then break end
      ar:data(buff)
   end
   fh:close()

   -- Test creating our own "normal" entry:
   local normal_entry = {
      fflags="nosappnd,dump,archive",
      dev=200,
      ino=1000,
      mode=0x80FF,
      nlink=3,
      uid=500,
      uname="u500",
      gid=500,
      gname="g500",
      rdev=100,
      atime=1257051679.0001,
      mtime=1257051678.0001,
      ctime=1257051677.0001,
      birthtime=1257051676.0001,
      size=100,
      sourcepath="source/test.txt",
      pathname="test.txt",
   }
   ar:header(archive.entry(normal_entry))
   ar:data("Test data")

   local symlink_entry = {
      symlink="test.txt",
      pathname="test.txt.sym",
   }
   ar:header(archive.entry(symlink_entry))

   local hardlink_entry = {
      hardlink="test.txt",
      pathname="test.txt.hard",
   }
   ar:header(archive.entry(hardlink_entry))

   ar:close()

   print("bytes_written=" .. bytes_written)

   collectgarbage("collect")
   ok(archive._write_ref_count() == 0,
      "write_ref_count=" .. tostring(archive._write_ref_count()) .. " gc works")
   ok(archive._entry_ref_count() == 0,
      "entry_ref_count=" .. tostring(archive._entry_ref_count()) .. " gc works")

   tmpfh:seek("set")

   local function reader(ar)
      return tmpfh:read(3)
   end

   ar = archive.read { reader = reader }

   local entry = ar:next_header()
   for key, value in pairs(normal_entry) do
      local got = entry[key](entry)
      ok(got == value, "checking " .. key ..
         " expect=" .. tostring(value) ..
         " got=" .. tostring(got))
   end

   local fh = assert(io.open(this_file, "rb"))
   local this_file_content = fh:read("*a")
   fh:close()

   local ar_file_content = {}
   while true do
      local data = ar:data()
      if nil == data then break end
      ar_file_content[#ar_file_content + 1] = data
   end
   ar_file_content = table.concat(ar_file_content)
   ok(ar_file_content == this_file_content, "this file matches archived file")

   for header in ar:headers() do
      print("pathame=[" ..header:pathname() .. "]")
   end
   ar:close()
   tmpfh:close()

   collectgarbage("collect")
   collectgarbage("collect")
   ok(archive._read_ref_count() == 0,
      "read_ref_count=" .. tostring(archive._read_ref_count()) .. " gc works")
   ok(archive._entry_ref_count() == 0,
      "entry_ref_count=" .. tostring(archive._entry_ref_count()) .. " gc works")

end


main()