print "1..41"

local src_dir, build_dir = ...
package.path  = src_dir .. "?.lua;" .. package.path
package.cpath = build_dir .. "?.so;" .. package.cpath

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
      format = "posix",
      bytes_in_last_block = 1,
      bytes_per_block = 100,
      options = "compression-level=9",
   }
   local fh = assert(io.open(this_file, "rb"))

   -- Test passing in a file name:
   ar:header(archive.entry {
                sourcepath = this_file,
                pathname = "test.lua",
             })
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
      atime={ 1257051679, 10 },
      mtime={ 1257051678, 20 },
      ctime={ 1257051677, 30 },
      birthtime={ 1257051676, 40 },
      size=9,
      pathname="test.txt",
   }
   ar:header(archive.entry(normal_entry))
   ar:data("Test data")

   -- TODO: Create special file so we can verify rdev works?

   local symlink_entry = {
      mode=0xA0FF, -- TODO: Make it so mode is set automatically?
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
   local header = ar:next_header()
   local expect_header = archive.entry { sourcepath = this_file }
   ok(header:pathname() == "test.lua",
      "check test.lua filename matches")
   ok(expect_header:mode() == header:mode(),
      string.format("check mode matches (%o=%o)",
                    expect_header:mode(), header:mode()))
   ok(expect_header:size() == header:size(),
      string.format("check size matches (%d=%d)",
                    expect_header:size(), header:size()))
   ok(expect_header:size() ~= 0, "non-zero size")

   header = nil -- so we can do gc check later on.
   expect_header = nil

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

   header_is(ar:next_header(), normal_entry)
   ok(ar:data() == "Test data", "gen content matches")
   ok(ar:data() == nil, "no more content")
   header_is(ar:next_header(), symlink_entry)
   ok(ar:data() == nil, "no content")
   header_is(ar:next_header(), hardlink_entry)
   ok(ar:data() == nil, "no content")

   ar:close()
   tmpfh:close()

   collectgarbage("collect")
   ok(archive._read_ref_count() == 0,
      "read_ref_count=" .. tostring(archive._read_ref_count()) .. " gc works")
   collectgarbage("collect")
   ok(archive._entry_ref_count() == 0,
      "entry_ref_count=" .. tostring(archive._entry_ref_count()) .. " gc works")

end

function header_is(got_header, expected_header)
   for key, value in pairs(expected_header) do
      local got = {got_header[key](got_header)}
      if ( type(value) == "table" ) then
         for i in ipairs(value) do
            ok(got[i] == value[i], "checking " .. key ..
               "[" .. i .. "] expect=" .. tostring(value[i]) ..
            " got=" .. tostring(got[i]))
         end
      else
         got = got[1]
         ok(got == value, "checking " .. key ..
            " expect=" .. tostring(value) ..
         " got=" .. tostring(got))
      end
   end
end

main()
tap.exit()
