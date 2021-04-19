local toolName = "TNS|Crossfire config|TNE"

---- #########################################################################
---- #                                                                       #
---- # Copyright (C) OpenTX                                                  #
-----#                                                                       #
---- # License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html               #
---- #                                                                       #
---- # This program is free software; you can redistribute it and/or modify  #
---- # it under the terms of the GNU General Public License version 2 as     #
---- # published by the Free Software Foundation.                            #
---- #                                                                       #
---- # This program is distributed in the hope that it will be useful        #
---- # but WITHOUT ANY WARRANTY; without even the implied warranty of        #
---- # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
---- # GNU General Public License for more details.                          #
---- #                                                                       #
---- #########################################################################



-- Init
local function init()
end

-- Run
local function run(event)
  if event == nil then
    error("Cannot run as a model script!")
    return 2
  end

  if crossfireTelemetryPush() == nil then
    error("Crossfire not available!")
    return 2
  end
  
  chdir("/SCRIPTS/TOOLS/CROSSFIRE")
  return "crossfire.lua"
end

return { init=init, run=run }
