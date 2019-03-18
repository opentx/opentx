
int ConvertTelemetrySource_216_to_217(int source)
{
  // TELEM_TIMER3 added
//  if (source >= TELEM_TIMER3)
  //  source += 1;

  return source;
}

#if defined(PCBTARANIS)
int ConvertSwitch_216_to_217(int swtch)
{
  if (swtch < 0)
    return -ConvertSwitch_216_to_217(-swtch);

  if (swtch > SWSRC_SF0)
    swtch += 1;

  if (swtch > SWSRC_SH0)
    swtch += 1;

  return swtch;
}
#else
int ConvertSwitch_216_to_217(int swtch)
{
  return swtch;
}
#endif

int ConvertSource_216_to_217(int source)
{
#if defined(PCBX9E)
  // SI to SR switches added
  if (source >= MIXSRC_SI)
    source += 10;
#endif
  // Telemetry conversions
  if (source >= MIXSRC_FIRST_TELEM)
    source = 0;

  return source;
}

int ConvertGVar_216_to_217(int value)
{
  if (value < -4096 + 9)
    value += 4096 - 1024;
  else if (value > 4095 - 9)
    value -= 4095 - 1023;
  return value;
}



void ConvertRadioData_216_to_217(RadioData & settings)
{
  RadioData_v216 * settings_v216 = (RadioData_v216 *)&settings;
  settings_v216->version = 217;
#if defined(PCBTARANIS)
  settings_v216->potsConfig = 0x05; // S1 and S2 = pots with detent
  settings_v216->switchConfig = 0x00007bff; // 6x3POS, 1x2POS, 1xTOGGLE
#endif
}


void ConvertModel_216_to_217(ModelData & model)
{
  // Timer3 added
  // 32bits Timers
  // MixData reduction
  // PPM center range
  // Telemetry custom screens

  assert(sizeof(ModelData_v216) <= sizeof(ModelData));

  ModelData_v216 oldModel;
  memcpy(&oldModel, &model, sizeof(oldModel));
  ModelData_v217 & newModel = (ModelData_v217 &)model;
  memset(&newModel, 0, sizeof(ModelData_v217));

  char name[LEN_MODEL_NAME+1];
  zchar2str(name, oldModel.header.name, LEN_MODEL_NAME);
  TRACE("Model %s conversion from v216 to v217", name);

  newModel.header.modelId[0] = oldModel.header.modelId;
  memcpy(newModel.header.name, oldModel.header.name, LEN_MODEL_NAME);
#if defined(PCBTARANIS) && LCD_W >= 212
  memcpy(newModel.header.bitmap, oldModel.header.bitmap, LEN_BITMAP_NAME);
#endif

  for (uint8_t i=0; i<2; i++) {
    TimerData_v217 & timer = newModel.timers[i];
    if (oldModel.timers[i].mode >= TMRMODE_COUNT)
      timer.mode = TMRMODE_COUNT + ConvertSwitch_216_to_217(oldModel.timers[i].mode - TMRMODE_COUNT + 1) - 1;
    else
      timer.mode = ConvertSwitch_216_to_217(oldModel.timers[i].mode);
    timer.start = oldModel.timers[i].start;
    timer.countdownBeep = oldModel.timers[i].countdownBeep;
    timer.minuteBeep = oldModel.timers[i].minuteBeep;
    timer.persistent = oldModel.timers[i].persistent;
    timer.value = oldModel.timers[i].value;
  }
  newModel.telemetryProtocol = oldModel.telemetryProtocol;
  newModel.thrTrim = oldModel.thrTrim;
  newModel.trimInc = oldModel.trimInc;
  newModel.disableThrottleWarning = oldModel.disableThrottleWarning;
  newModel.displayChecklist = oldModel.displayChecklist;
  newModel.extendedLimits = oldModel.extendedLimits;
  newModel.extendedTrims = oldModel.extendedTrims;
  newModel.throttleReversed = oldModel.throttleReversed;
  newModel.beepANACenter = oldModel.beepANACenter;
  for (int i=0; i<MAX_MIXERS; i++) {
    newModel.mixData[i].destCh = oldModel.mixData[i].destCh;
    newModel.mixData[i].flightModes = oldModel.mixData[i].flightModes;
    newModel.mixData[i].mltpx = oldModel.mixData[i].mltpx;
    newModel.mixData[i].carryTrim = oldModel.mixData[i].carryTrim;
    newModel.mixData[i].mixWarn = oldModel.mixData[i].mixWarn;
    newModel.mixData[i].weight = ConvertGVar_216_to_217(oldModel.mixData[i].weight);
    newModel.mixData[i].swtch = ConvertSwitch_216_to_217(oldModel.mixData[i].swtch);
#if defined(PCBTARANIS)
    newModel.mixData[i].curve = oldModel.mixData[i].curve;
#else
    // TODO newModel.mixData[i].curveMode = oldModel.mixData[i].curveMode;
    // TODO newModel.mixData[i].noExpo = oldModel.mixData[i].noExpo;
    // TODO newModel.mixData[i].curveParam = oldModel.mixData[i].curveParam;
#endif
    newModel.mixData[i].delayUp = oldModel.mixData[i].delayUp;
    newModel.mixData[i].delayDown = oldModel.mixData[i].delayDown;
    newModel.mixData[i].speedUp = oldModel.mixData[i].speedUp;
    newModel.mixData[i].speedDown = oldModel.mixData[i].speedDown;
    newModel.mixData[i].srcRaw = ConvertSource_216_to_217(oldModel.mixData[i].srcRaw);
    newModel.mixData[i].offset = ConvertGVar_216_to_217(oldModel.mixData[i].offset);
    memcpy(newModel.mixData[i].name, oldModel.mixData[i].name, sizeof(newModel.mixData[i].name));
  }
  for (int i=0; i<MAX_OUTPUT_CHANNELS; i++) {
#if defined(PCBTARANIS)
    newModel.limitData[i].min = ConvertGVar_216_to_217(oldModel.limitData[i].min);
    newModel.limitData[i].max = ConvertGVar_216_to_217(oldModel.limitData[i].max);
    newModel.limitData[i].offset = ConvertGVar_216_to_217(oldModel.limitData[i].offset);
    newModel.limitData[i].ppmCenter = oldModel.limitData[i].ppmCenter;
    newModel.limitData[i].symetrical = oldModel.limitData[i].symetrical;
    newModel.limitData[i].revert = oldModel.limitData[i].revert;
    newModel.limitData[i].curve = oldModel.limitData[i].curve;
    memcpy(newModel.limitData[i].name, oldModel.limitData[i].name, sizeof(newModel.limitData[i].name));
#else
    newModel.limitData[i] = oldModel.limitData[i];
#endif
  }
  for (int i=0; i<MAX_EXPOS; i++) {
#if defined(PCBTARANIS)
    newModel.expoData[i].srcRaw = ConvertSource_216_to_217(oldModel.expoData[i].srcRaw);
    newModel.expoData[i].scale = oldModel.expoData[i].scale;
    newModel.expoData[i].carryTrim = oldModel.expoData[i].carryTrim;
    newModel.expoData[i].curve = oldModel.expoData[i].curve;
    newModel.expoData[i].offset = oldModel.expoData[i].offset;
#else
    newModel.expoData[i].curveMode = oldModel.expoData[i].curveMode;
    newModel.expoData[i].curveParam = oldModel.expoData[i].curveParam;
#endif
    newModel.expoData[i].chn = oldModel.expoData[i].chn;
    newModel.expoData[i].swtch = ConvertSwitch_216_to_217(oldModel.expoData[i].swtch);
    newModel.expoData[i].flightModes = oldModel.expoData[i].flightModes;
    newModel.expoData[i].weight = oldModel.expoData[i].weight;
    newModel.expoData[i].mode = oldModel.expoData[i].mode;
    memcpy(newModel.expoData[i].name, oldModel.expoData[i].name, sizeof(newModel.expoData[i].name));
  }
  memcpy(newModel.curves, oldModel.curves, sizeof(newModel.curves));
  memcpy(newModel.points, oldModel.points, sizeof(newModel.points));
  for (int i=0; i<32; i++) {
    LogicalSwitchData_v217 & sw = newModel.logicalSw[i];
    sw.func = oldModel.logicalSw[i].func;
    sw.v1 = oldModel.logicalSw[i].v1;
    sw.v2 = oldModel.logicalSw[i].v2;
    sw.v3 = oldModel.logicalSw[i].v3;
    sw.delay = oldModel.logicalSw[i].delay;
    sw.duration = oldModel.logicalSw[i].duration;
    uint8_t cstate = lswFamily(sw.func);
    if (cstate == LS_FAMILY_OFS || cstate == LS_FAMILY_COMP || cstate == LS_FAMILY_DIFF) {
      sw.v1 = ConvertSource_216_to_217((uint8_t)sw.v1);
      if (cstate == LS_FAMILY_COMP) {
        sw.v2 = ConvertSource_216_to_217((uint8_t)sw.v2);
      }
    }
    else if (cstate == LS_FAMILY_BOOL || cstate == LS_FAMILY_STICKY) {
      sw.v1 = ConvertSwitch_216_to_217(sw.v1);
      sw.v2 = ConvertSwitch_216_to_217(sw.v2);
    }
    else if (cstate == LS_FAMILY_EDGE) {
      sw.v1 = ConvertSwitch_216_to_217(sw.v1);
    }
    sw.andsw = ConvertSwitch_216_to_217(sw.andsw);
  }
  for (int i=0; i<MAX_SPECIAL_FUNCTIONS; i++) {
    CustomFunctionData_v216 & fn = newModel.customFn[i];
    fn = oldModel.customFn[i];
    fn.swtch = ConvertSwitch_216_to_217(fn.swtch);
    if (fn.func == FUNC_PLAY_VALUE || fn.func == FUNC_VOLUME || (IS_ADJUST_GV_FUNC(fn.func) && fn.all.mode == FUNC_ADJUST_GVAR_SOURCE)) {
      fn.all.val = ConvertSource_216_to_217(fn.all.val);
    }
  }

  newModel.swashR.collectiveSource = ConvertSource_216_to_217(newModel.swashR.collectiveSource);
  // TODO other fields

  for (int i=0; i<MAX_FLIGHT_MODES; i++) {
    newModel.flightModeData[i] = oldModel.flightModeData[i];
    newModel.flightModeData[i].swtch = ConvertSwitch_216_to_217(oldModel.flightModeData[i].swtch);
  }

  newModel.thrTraceSrc = oldModel.thrTraceSrc;
  newModel.switchWarningState = oldModel.switchWarningState;
  newModel.switchWarningEnable = oldModel.switchWarningEnable;
  memcpy(newModel.gvars, oldModel.gvars, sizeof(newModel.gvars));

  memcpy(&newModel.rssiAlarms, &oldModel.frsky.rssiAlarms, sizeof(newModel.rssiAlarms));

  for (int i=0; i<NUM_MODULES+1; i++) {
    newModel.moduleData[i].type = 0;
    newModel.moduleData[i].rfProtocol = oldModel.moduleData[i].rfProtocol;
    newModel.moduleData[i].channelsStart = oldModel.moduleData[i].channelsStart;
    newModel.moduleData[i].channelsCount = oldModel.moduleData[i].channelsCount;
    newModel.moduleData[i].failsafeMode = oldModel.moduleData[i].failsafeMode + 1;
    for (int j=0; j<MAX_OUTPUT_CHANNELS; j++) {
      newModel.moduleData[i].failsafeChannels[j] = oldModel.moduleData[i].failsafeChannels[j];
    }
    newModel.moduleData[i].ppm.delay = oldModel.moduleData[i].ppmDelay;
    newModel.moduleData[i].ppm.frameLength = oldModel.moduleData[i].ppmFrameLength;
    newModel.moduleData[i].ppm.pulsePol = oldModel.moduleData[i].ppmPulsePol;
  }

#if defined(PCBTARANIS)
  newModel.moduleData[INTERNAL_MODULE].type = MODULE_TYPE_XJT;
#endif
  newModel.moduleData[EXTERNAL_MODULE].type = oldModel.externalModule;

#if defined(PCBTARANIS)
  newModel.trainerMode = oldModel.trainerMode;
  // TODO memcpy(newModel.scriptsData, oldModel.scriptsData, sizeof(newModel.scriptsData));
  memcpy(newModel.curveNames, oldModel.curveNames, sizeof(newModel.curveNames));
  memcpy(newModel.inputNames, oldModel.inputNames, sizeof(newModel.inputNames));
#endif
  newModel.potsWarnMode = oldModel.nPotsToWarn >> 6;
  newModel.potsWarnEnabled = oldModel.nPotsToWarn & 0x1f;
  memcpy(newModel.potsWarnPosition, oldModel.potPosition, sizeof(newModel.potsWarnPosition));
}
