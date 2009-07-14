print "1..6"

local tap   = require("tap")
local ar    = require("archive")
local ok    = tap.ok
local io    = require("io")

function main()
   test_basic()
end

function test_basic()
   local tfile = io.tmpfile()
   ar.write { printer = function(str) tfile:write(str) end }

   -- TODO: Implement me!
end


main()