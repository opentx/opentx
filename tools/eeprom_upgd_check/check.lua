local function checktable(actual, reference)
  for k, v in pairs( actual ) do
    if v == reference[k] then
      print("OK ", k, v, reference[k])
    else
      print("ISSUE ", k, v, reference[k])
    end
  end
end

local function run(event)
  inputt= {name = "In1", source  = 1, weight = 50, offset = 10, switch = 3}
  mixt= {name = "Mix1", source = 1, weight = 50, offset = 10, curveType = 1, curveValue = 50, flightModes = 3, delayUp = 5, speedDown = 5, speedUp =  2, delayDown = 5, mixWarn = 1 , carryTrim = true , multiplex = 1, switch = 3}
  outputt = {name = "Out1", min = -800, max = 800, offset = 10, ppmCenter = 10, symetrical = 1, revert = 1, switch = 3}
  timert = {mode = 4, start = 0, countdownBeep = 2, persistent = 1, minuteBeep = true, value = 10 }
  lst = {func = 2, v1 = 2, v2 = 2, v3 = 2, duration = 3, delay = 2, ["and"] = 2}
  input = model.getInput(4,0)
  mix = model.getMix(4,0)
  output = model.getOutput(4)
  ls = model.getLogicalSwitch(0)
  timer = model.getTimer(0)
  checktable(input,inputt)
  checktable(mix, mixt)
  checktable(output, outputt)
  checktable(ls, lst)
  checktable(timer, timert)
  return 2
end
return {run=run}
