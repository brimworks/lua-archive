print "1..6"

local tap     = require("tap")
local archive = require("archive")
local ok      = tap.ok
local io      = require("io")

function main()
   test_basic()
end

function test_basic()
   local this_file = string.sub(debug.getinfo(1,'S').source, 2)

   local tmpfh = io.tmpfile() --open("/tmp/test.tar", "wb")
   local function printer(ar, str)
      if ( nil == str ) then
         tmpfh:flush()
      else
         tmpfh:write(str)
         return #str
      end
   end
   local ar = archive.write { printer = printer }
   local fh = io.open(this_file, "rb")

   -- Test passing in a file name:
   ar:header(archive.entry(this_file))
   while ( true ) do
      local buff = fh:read(10000)
      if ( nil == buff ) then break end
      ar:data(buff)
   end
   fh:close()

   -- Test creating our own entry:
   local test_entry = {
      fflags="nosappnd,dump,archive",
      dev=200,
      ino=1000,
      mode=0xA000,
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
      symlink="test.txt.sym",
      hardlink="test.txt.hard",
      pathname="test.txt",
   }
   ar:header(archive.entry(test_entry))
   ar:data("Test data")

   ar:close()

   collectgarbage("collect")
   ok(archive._write_ref_count() == 0,
      "ref_count=" .. tostring(archive._write_ref_count()) .. " gc works")
   ok(archive._entry_ref_count() == 0,
      "ref_count=" .. tostring(archive._entry_ref_count()) .. " gc works")

   tmpfh:seek("set")

-- TODO: Validate by reading the archive:
--   archive.read()

end


main()