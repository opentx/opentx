
int ConvertSwitch_217_to_218(int swtch)
{
  // 32 additional logical switches

  if (swtch < 0)
    return -ConvertSwitch_217_to_218(-swtch);

  if (swtch >= SWSRC_FIRST_LOGICAL_SWITCH+32)
    return swtch+32;

  return swtch;
}



int ConvertSource_217_to_218(int source)
{
#if defined(PCBTARANIS)
  if (source >= MIXSRC_FIRST_LOGICAL_SWITCH + 32)
    source += 32;
#endif

  return source;
}



void ConvertSpecialFunctions_217_to_218(CustomFunctionData * cf218, CustomFunctionData_v216 * cf216)
{
  for (int i=0; i<MAX_SPECIAL_FUNCTIONS; i++) {
    CustomFunctionData & cf = cf218[i];
    memcpy(&cf, &cf216[i], sizeof(CustomFunctionData));
    cf.swtch = ConvertSwitch_217_to_218(cf216[i].swtch);
    cf.func = cf216[i].func;
    if (cf.func == FUNC_PLAY_VALUE || cf.func == FUNC_VOLUME || (IS_ADJUST_GV_FUNC(cf.func) && cf.all.mode == FUNC_ADJUST_GVAR_SOURCE)) {
      cf.all.val = ConvertSource_217_to_218(cf.all.val);
    }
  }
}



void ConvertRadioData_217_to_218(RadioData & settings)
{
  RadioData_v216 settings_v217 = (RadioData_v216 &)settings;

  settings.version = 218;
#if !defined(PCBTARANIS)
  settings.stickReverse = settings_v217.stickReverse;
#endif
  settings.beepLength = settings_v217.beepLength;
  settings.hapticStrength = settings_v217.hapticStrength;
  settings.gpsFormat = settings_v217.gpsFormat;
  settings.unexpectedShutdown = settings_v217.unexpectedShutdown;
  settings.speakerPitch = settings_v217.speakerPitch;
  settings.speakerVolume = settings_v217.speakerVolume;
  settings.vBatMin = settings_v217.vBatMin;
  settings.vBatMax = settings_v217.vBatMax;
  settings.backlightBright = settings_v217.backlightBright;
  settings.globalTimer = settings_v217.globalTimer;
  settings.bluetoothBaudrate = settings_v217.bluetoothBaudrate;
  settings.countryCode = settings_v217.countryCode;
  settings.imperial = settings_v217.imperial;
  settings.ttsLanguage[0] = settings_v217.ttsLanguage[0];
  settings.ttsLanguage[1] = settings_v217.ttsLanguage[1];
  settings.beepVolume = settings_v217.beepVolume;
  settings.wavVolume = settings_v217.wavVolume;
  settings.varioVolume = settings_v217.varioVolume;
  settings.backgroundVolume = settings_v217.backgroundVolume;
  settings.varioPitch = settings_v217.varioPitch;
  settings.varioRange = settings_v217.varioRange;
  settings.varioRepeat = settings_v217.varioRepeat;
  ConvertSpecialFunctions_217_to_218(settings.customFn, settings_v217.customFn);

#if defined(PCBTARANIS)
  settings.serial2Mode = settings_v217.serial2Mode;
  settings.slidersConfig = settings_v217.slidersConfig;
  settings.potsConfig = settings_v217.potsConfig;
  settings.backlightColor = settings_v217.backlightColor;
  settings.switchUnlockStates = settings_v217.switchUnlockStates;
  settings.switchConfig = settings_v217.switchConfig;
  memcpy(settings.switchNames, settings_v217.switchNames, sizeof(settings.switchNames));
  memcpy(settings.anaNames, settings_v217.anaNames, sizeof(settings.anaNames));
#endif

#if defined(PCBX9E)
  memcpy(settings.bluetoothName, settings_v217.bluetoothName, sizeof(settings.bluetoothName));
#endif

#if defined(PCBSKY9X)
  settings.txCurrentCalibration = settings_v217.txCurrentCalibration;
  settings.temperatureWarn = settings_v217.temperatureWarn;
  settings.mAhWarn = settings_v217.mAhWarn;
  settings.mAhUsed = settings_v217.mAhUsed;
  settings.temperatureCalib = settings_v217.temperatureCalib;
  settings.optrexDisplay = settings_v217.optrexDisplay;
  settings.sticksGain = settings_v217.sticksGain;
  settings.rotarySteps = settings_v217.rotarySteps;
#endif
}


void ConvertModel_217_to_218(ModelData & model)
{
  assert(sizeof(ModelData_v217) <= sizeof(ModelData));

  ModelData_v217 oldModel;
  memcpy(&oldModel, &model, sizeof(oldModel));
  ModelData & newModel = model;
  memset(&newModel, 0, sizeof(ModelData));

  char name[LEN_MODEL_NAME+1];
  zchar2str(name, oldModel.header.name, LEN_MODEL_NAME);
  TRACE("Model %s conversion from v217 to v218", name);

  newModel.header = oldModel.header;
  for (uint8_t i=0; i<MAX_TIMERS; i++) {
    if (oldModel.timers[i].mode >= TMRMODE_COUNT)
      newModel.timers[i].mode = TMRMODE_COUNT + ConvertSwitch_217_to_218(oldModel.timers[i].mode - TMRMODE_COUNT + 1) - 1;
    else
      newModel.timers[i].mode = ConvertSwitch_217_to_218(oldModel.timers[i].mode);
    if (oldModel.timers[i].mode)
      TRACE("timer mode %d => %d", oldModel.timers[i].mode, newModel.timers[i].mode);
    newModel.timers[i].start = oldModel.timers[i].start;
    newModel.timers[i].value = oldModel.timers[i].value;
    newModel.timers[i].countdownBeep = oldModel.timers[i].countdownBeep;
    newModel.timers[i].minuteBeep = oldModel.timers[i].minuteBeep;
    newModel.timers[i].persistent = oldModel.timers[i].persistent;
    memcpy(newModel.timers[i].name, oldModel.timers[i].name, sizeof(newModel.timers[i].name));
  }
  newModel.telemetryProtocol = oldModel.telemetryProtocol;
  newModel.thrTrim = oldModel.thrTrim;
  newModel.noGlobalFunctions = oldModel.noGlobalFunctions;
  newModel.displayTrims = oldModel.displayTrims;
  newModel.ignoreSensorIds = oldModel.ignoreSensorIds;
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
    newModel.mixData[i].weight = oldModel.mixData[i].weight;
    newModel.mixData[i].swtch = ConvertSwitch_217_to_218(oldModel.mixData[i].swtch);
#if defined(PCBTARANIS)
    newModel.mixData[i].curve = oldModel.mixData[i].curve;
#else
    // newModel.mixData[i].curveMode = oldModel.mixData[i].curveMode;
    // newModel.mixData[i].noExpo = oldModel.mixData[i].noExpo;
    // newModel.mixData[i].curveParam = oldModel.mixData[i].curveParam;
#endif
    newModel.mixData[i].delayUp = oldModel.mixData[i].delayUp;
    newModel.mixData[i].delayDown = oldModel.mixData[i].delayDown;
    newModel.mixData[i].speedUp = oldModel.mixData[i].speedUp;
    newModel.mixData[i].speedDown = oldModel.mixData[i].speedDown;
    newModel.mixData[i].srcRaw = ConvertSource_217_to_218(oldModel.mixData[i].srcRaw);
    newModel.mixData[i].offset = oldModel.mixData[i].offset;
    memcpy(newModel.mixData[i].name, oldModel.mixData[i].name, sizeof(newModel.mixData[i].name));
  }
  for (int i=0; i<MAX_OUTPUT_CHANNELS; i++) {
    newModel.limitData[i] = oldModel.limitData[i];
#if defined(PCBTARANIS)
    if (newModel.moduleData[INTERNAL_MODULE].type == MODULE_TYPE_XJT || newModel.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_XJT) {
      newModel.limitData[i].ppmCenter = (oldModel.limitData[i].ppmCenter * 612) / 1024;
    }
#endif
  }
  for (int i=0; i<MAX_EXPOS; i++) {
#if defined(PCBTARANIS)
    newModel.expoData[i].srcRaw = ConvertSource_217_to_218(oldModel.expoData[i].srcRaw);
    newModel.expoData[i].scale = oldModel.expoData[i].scale;
    newModel.expoData[i].carryTrim = oldModel.expoData[i].carryTrim;
    newModel.expoData[i].curve = oldModel.expoData[i].curve;
    newModel.expoData[i].offset = oldModel.expoData[i].offset;
#else
    // TODO newModel.expoData[i].curveMode = oldModel.expoData[i].curveMode;
    // TODO newModel.expoData[i].curveParam = oldModel.expoData[i].curveParam;
#endif
    newModel.expoData[i].chn = oldModel.expoData[i].chn;
    newModel.expoData[i].swtch = ConvertSwitch_217_to_218(oldModel.expoData[i].swtch);
    newModel.expoData[i].flightModes = oldModel.expoData[i].flightModes;
    newModel.expoData[i].weight = oldModel.expoData[i].weight;
    newModel.expoData[i].mode = oldModel.expoData[i].mode;
    memcpy(newModel.expoData[i].name, oldModel.expoData[i].name, sizeof(newModel.expoData[i].name));
  }
  for (int i=0; i<MAX_CURVES; i++) {
#if defined(PCBTARANIS)
    newModel.curves[i].type = oldModel.curves[i].type;
    newModel.curves[i].smooth = oldModel.curves[i].smooth;
    newModel.curves[i].points = oldModel.curves[i].points;
    memcpy(newModel.curves[i].name, oldModel.curveNames[i], sizeof(newModel.curves[i].name));
#else
    // TODO newModel.curves[i] = oldModel.curves[i];
#endif
  }
  memcpy(newModel.points, oldModel.points, sizeof(newModel.points));
  for (int i=0; i<32; i++) {
    LogicalSwitchData & sw = newModel.logicalSw[i];
    sw.func = oldModel.logicalSw[i].func;
    sw.v1 = oldModel.logicalSw[i].v1;
    sw.v2 = oldModel.logicalSw[i].v2;
    sw.v3 = oldModel.logicalSw[i].v3;
    newModel.logicalSw[i].andsw = ConvertSwitch_217_to_218(oldModel.logicalSw[i].andsw);
    sw.delay = oldModel.logicalSw[i].delay;
    sw.duration = oldModel.logicalSw[i].duration;
    uint8_t cstate = lswFamily(sw.func);
    if (cstate == LS_FAMILY_OFS || cstate == LS_FAMILY_COMP || cstate == LS_FAMILY_DIFF) {
      sw.v1 = ConvertSource_217_to_218((uint8_t)sw.v1);
      if (cstate == LS_FAMILY_COMP) {
        sw.v2 = ConvertSource_217_to_218((uint8_t)sw.v2);
      }
    }
    else if (cstate == LS_FAMILY_BOOL || cstate == LS_FAMILY_STICKY) {
      sw.v1 = ConvertSwitch_217_to_218(sw.v1);
      sw.v2 = ConvertSwitch_217_to_218(sw.v2);
    }
    else if (cstate == LS_FAMILY_EDGE) {
      sw.v1 = ConvertSwitch_217_to_218(sw.v1);
    }
  }
  ConvertSpecialFunctions_217_to_218(newModel.customFn, oldModel.customFn);
  newModel.swashR = oldModel.swashR;
  for (int i=0; i<MAX_FLIGHT_MODES; i++) {
    memcpy(newModel.flightModeData[i].trim, oldModel.flightModeData[i].trim, sizeof(newModel.flightModeData[i].trim));
    memcpy(newModel.flightModeData[i].name, oldModel.flightModeData[i].name, sizeof(newModel.flightModeData[i].name));
    newModel.flightModeData[i].swtch = ConvertSwitch_217_to_218(oldModel.flightModeData[i].swtch);
    newModel.flightModeData[i].fadeIn = oldModel.flightModeData[i].fadeIn;
    newModel.flightModeData[i].fadeOut = oldModel.flightModeData[i].fadeOut;
#if defined(PCBSKY9X)
    memcpy(newModel.flightModeData[i].rotaryEncoders, oldModel.flightModeData[i].rotaryEncoders, sizeof(newModel.flightModeData[i].rotaryEncoders));
#endif
    memcpy(newModel.flightModeData[i].gvars, oldModel.flightModeData[i].gvars, sizeof(newModel.flightModeData[i].gvars));
  }
  newModel.thrTraceSrc = oldModel.thrTraceSrc;
  newModel.switchWarningState = oldModel.switchWarningState;
  newModel.switchWarningEnable = oldModel.switchWarningEnable;
  for (int i=0; i<MAX_GVARS; i++) {
    memcpy(newModel.gvars[i].name, oldModel.gvars[i].name, sizeof(newModel.gvars[i].name));
    newModel.gvars[i].popup = oldModel.gvars[i].popup;
  }
  newModel.frsky = oldModel.frsky;
  for (int i=0; i<MAX_TELEMETRY_SCREENS; i++) {
    if (((oldModel.frsky.screensType >> (2*i)) & 0x03) == TELEMETRY_SCREEN_TYPE_VALUES) {
      for (int j = 0; j < 4; j++) {
        for (int k = 0; k < NUM_LINE_ITEMS; k++) {
          newModel.frsky.screens[i].lines[j].sources[k] = ConvertSource_217_to_218(oldModel.frsky.screens[i].lines[j].sources[k]);
        }
      }
    }
    else if (((oldModel.frsky.screensType >> (2*i)) & 0x03) == TELEMETRY_SCREEN_TYPE_GAUGES) {
      for (int j = 0; j < 4; j++) {
        newModel.frsky.screens[i].bars[j].source = ConvertSource_217_to_218(oldModel.frsky.screens[i].bars[j].source);
      }
    }
  }
  for (int i=0; i<NUM_MODULES+1; i++) {
    // newModel.moduleData[i] = oldModel.moduleData[i];
#warning "Conversion from v217 to v218 is broken"
  }
#if defined(PCBTARANIS)
  newModel.trainerData.mode = oldModel.trainerMode;
  memcpy(newModel.scriptsData, oldModel.scriptsData, sizeof(newModel.scriptsData));
  memcpy(newModel.inputNames, oldModel.inputNames, sizeof(newModel.inputNames));
#endif
  newModel.potsWarnMode = oldModel.potsWarnMode;
  newModel.potsWarnEnabled = oldModel.potsWarnEnabled;
  memcpy(newModel.potsWarnPosition, oldModel.potsWarnPosition, sizeof(newModel.potsWarnPosition));
  for (uint8_t i=0; i<MAX_TELEMETRY_SENSORS; i++) {
    newModel.telemetrySensors[i] = oldModel.telemetrySensors[i];
    if (newModel.telemetrySensors[i].unit > UNIT_WATTS)
      newModel.telemetrySensors[i].unit += 1;
  }
#if defined(PCBX9E)
  newModel.toplcdTimer = oldModel.toplcdTimer;
#endif
}

