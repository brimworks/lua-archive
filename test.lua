print "1..6"

local tap   = require("tap")
local ar    = require("archive")
local ok    = tap.ok
local io    = require("io")

function main()
   test_basic()
end

function test_basic()
--   local tfile = io.tmpfile()
   local function printer(ar, str)
      if ( nil == str ) then
         io.stderr:write("close()\n")
      else
         io.stderr:write("print(" .. #str .. " chars)\n")
      end
   end
   ar.write { printer = printer }
--[[         printer = function(str)
            tfile:write(str)
         end
      }
]]--
   collectgarbage("collect")
   ok(ar._write_ref_count() == 0, ("ref_count=" .. tostring(ar._write_ref_count()) .. " gc works"))
   -- TODO: Implement me!
end


main()