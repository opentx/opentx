#include "gtests.h"
#include "location.h"
#include "storage/otx.h"
#include "storage/storage.h"
#include "firmwares/opentx/opentxinterface.h"
#include "firmwares/customfunctiondata.h"

TEST(Conversions, ConversionX9DPFrom22)
{
  RadioData radioData;
  Storage   store = Storage(RADIO_TESTS_PATH "/eeprom_22_x9d+.bin");

  ASSERT_EQ(true, store.load(radioData));
  
  ASSERT_EQ(true, store.load(radioData));

  const GeneralSettings& settings = radioData.generalSettings;
  EXPECT_EQ(RawSwitch(SWITCH_TYPE_TELEMETRY, 1), settings.customFn[0].swtch);
  EXPECT_EQ(FuncLogs, settings.customFn[0].func);
  EXPECT_EQ(20, settings.customFn[0].param);

  EXPECT_STREQ("Tes", settings.switchName[0]);
  EXPECT_EQ(Board::SWITCH_3POS, settings.switchConfig[0]);
  
  const ModelData& model = radioData.models[0];
  EXPECT_STREQ("Test", model.name);
  EXPECT_EQ(RawSwitch(SWITCH_TYPE_SWITCH, SWITCH_SA0), model.timers[0].mode);
  EXPECT_EQ(80, model.mixData[0].weight);
  EXPECT_EQ(900, model.limitData[0].max); // -100
  EXPECT_EQ(80, model.expoData[0].weight);
  EXPECT_EQ(HELI_SWASH_TYPE_120X, model.swashRingData.type);
  EXPECT_EQ(10, model.flightModeData[0].gvars[0]);
  EXPECT_STREQ("Tes", model.gvarData[0].name);
  EXPECT_EQ(PULSES_PXX_XJT_X16, model.moduleData[0].protocol);
  EXPECT_EQ(0, model.moduleData[0].subType);
  EXPECT_EQ(PULSES_PXX_R9M, model.moduleData[1].protocol);
  EXPECT_EQ(MODULE_SUBTYPE_R9M_FCC, model.moduleData[1].subType);
  EXPECT_STREQ("Thr", model.inputNames[0]);
  EXPECT_STREQ("Tes", model.sensorData[0].label);
  EXPECT_EQ(10, model.sensorData[0].id);
  EXPECT_EQ(9, model.sensorData[0].instance);
  EXPECT_EQ(RawSource(SOURCE_TYPE_TELEMETRY,0).toValue(), model.logicalSw[0].val1);
}

TEST(Conversions, ConversionX7From22)
{
  RadioData radioData;
  Storage   store = Storage(RADIO_TESTS_PATH "/eeprom_22_x7.bin");

  ASSERT_EQ(true, store.load(radioData));

  const GeneralSettings& settings = radioData.generalSettings;
  EXPECT_EQ(RawSwitch(SWITCH_TYPE_TELEMETRY,1), settings.customFn[0].swtch);
  EXPECT_EQ(FuncLogs, settings.customFn[0].func);
  EXPECT_EQ(20, settings.customFn[0].param);

  EXPECT_STREQ("Tes", settings.switchName[0]);
  EXPECT_EQ(Board::SWITCH_3POS, settings.switchConfig[0]);
  
  const ModelData& model = radioData.models[0];
  EXPECT_STREQ("Test", model.name);
  EXPECT_EQ(PULSES_PXX_R9M, model.moduleData[1].protocol);
  EXPECT_EQ(MODULE_SUBTYPE_R9M_EU, model.moduleData[1].subType);

  EXPECT_EQ(80, model.mixData[0].weight);
  EXPECT_EQ(80, model.expoData[0].weight);

  EXPECT_EQ(RawSource(SOURCE_TYPE_PPM,0), model.mixData[4].srcRaw);
  EXPECT_EQ(RawSwitch(SWITCH_TYPE_TELEMETRY,1), model.mixData[4].swtch);
  EXPECT_EQ(HELI_SWASH_TYPE_120X, model.swashRingData.type);
  EXPECT_STREQ("Thr", model.inputNames[0]);

  EXPECT_STREQ("Tes", model.sensorData[0].label);
  EXPECT_EQ(10, model.sensorData[0].id);
  EXPECT_EQ(9, model.sensorData[0].instance);
  EXPECT_EQ(900, model.limitData[0].max); // -100

  EXPECT_EQ(10, model.flightModeData[0].gvars[0]);
  EXPECT_STREQ("FMtest", model.flightModeData[1].name);
  EXPECT_EQ(RawSwitch(SWITCH_TYPE_VIRTUAL,1), model.flightModeData[1].swtch);
  EXPECT_STREQ("Tes", model.gvarData[0].name);

  EXPECT_EQ(LS_FN_VPOS, model.logicalSw[0].func);
  EXPECT_EQ(RawSource(SOURCE_TYPE_PPM,0).toValue(), model.logicalSw[0].val1);
  EXPECT_EQ(0, model.logicalSw[0].val2);

  const FrSkyScreenData& screen = model.frsky.screens[0];
  EXPECT_EQ(TELEMETRY_SCREEN_NUMBERS, screen.type);
  EXPECT_EQ(RawSource(SOURCE_TYPE_PPM,0), screen.body.lines[0].source[0]);
  EXPECT_EQ(RawSource(SOURCE_TYPE_SPECIAL,4), screen.body.lines[0].source[1]);
}

TEST(Conversions, ConversionXLiteFrom22)
{
  RadioData radioData;
  Storage   store = Storage(RADIO_TESTS_PATH "/eeprom_22_xlite.bin");

  ASSERT_EQ(true, store.load(radioData));

  const GeneralSettings& settings = radioData.generalSettings;
  EXPECT_EQ(RawSwitch(SWITCH_TYPE_TELEMETRY,1), settings.customFn[0].swtch);
  EXPECT_EQ(FuncLogs, settings.customFn[0].func);
  EXPECT_EQ(20, settings.customFn[0].param);

  EXPECT_STREQ("Tes", settings.switchName[0]);
  EXPECT_EQ(Board::SWITCH_3POS, settings.switchConfig[0]);

  const ModelData& model = radioData.models[0];
  EXPECT_STREQ("Test", model.name);
  EXPECT_EQ(PULSES_PXX_R9M, model.moduleData[1].protocol);
  EXPECT_EQ(MODULE_SUBTYPE_R9M_EU, model.moduleData[1].subType);

  EXPECT_EQ(80, model.mixData[0].weight);
  EXPECT_EQ(80, model.expoData[0].weight);

  EXPECT_EQ(RawSource(SOURCE_TYPE_PPM,0), model.mixData[4].srcRaw);
  EXPECT_EQ(RawSwitch(SWITCH_TYPE_TELEMETRY, 1), model.mixData[4].swtch);
  EXPECT_EQ(HELI_SWASH_TYPE_120X, model.swashRingData.type);
  EXPECT_STREQ("Thr", model.inputNames[0]);

  EXPECT_STREQ("Tes", model.sensorData[0].label);
  EXPECT_EQ(10, model.sensorData[0].id);
  EXPECT_EQ(8, model.sensorData[0].instance);
  EXPECT_EQ(900, model.limitData[0].max); // -100

  EXPECT_EQ(LS_FN_VPOS, model.logicalSw[0].func);
  EXPECT_EQ(RawSource(SOURCE_TYPE_PPM,0).toValue(), model.logicalSw[0].val1);
  EXPECT_EQ(0, model.logicalSw[0].val2);

  const FrSkyScreenData& screen = model.frsky.screens[0];
  EXPECT_EQ(TELEMETRY_SCREEN_NUMBERS, screen.type);
  EXPECT_EQ(RawSource(SOURCE_TYPE_PPM,0), screen.body.lines[0].source[0]);
  EXPECT_EQ(RawSource(SOURCE_TYPE_SPECIAL,4), screen.body.lines[0].source[1]);
}


bool loadFile(QByteArray & filedata, const QString & filename)
{
  QFile file(filename);
  if (!file.open(QFile::ReadOnly)) {
    qDebug() << QString("Error opening file %1:\n%2.").arg(filename).arg(file.errorString());
    return false;
  }
  filedata = file.readAll();
  qDebug() << "File" << filename << "read, size:" << filedata.size();
  return true;
}

TEST(Conversions, ConversionX10From22)
{
  QByteArray byteBuffer;

#define USE_OTX

#if defined(USE_OTX)
  OtxFormat otx(RADIO_TESTS_PATH "/model_22_x10.otx");
  RadioData radio;
  EXPECT_EQ(true, otx.load(radio));

  const GeneralSettings& settings = radio.generalSettings;
  const ModelData& model = radio.models[0];
#else  
  ASSERT_EQ(true, loadFile(byteBuffer, RADIO_TESTS_PATH "/model_22_x10/RADIO/radio.bin"));

  GeneralSettings settings;
  EXPECT_NE(nullptr, loadRadioSettingsFromByteArray(settings, byteBuffer));
#endif

  EXPECT_EQ(100, settings.calibSpanNeg[9]);
  EXPECT_EQ(500, settings.calibMid[9]);
  EXPECT_EQ(900, settings.calibSpanPos[9]);

  EXPECT_EQ(200, settings.calibSpanNeg[10]);
  EXPECT_EQ(400, settings.calibMid[10]);
  EXPECT_EQ(600, settings.calibSpanPos[10]);

  EXPECT_EQ(-23, settings.vBatMin);
  EXPECT_EQ(20, settings.speakerVolume);
  EXPECT_STREQ("en", settings.ttsLanguage);
  EXPECT_STREQ("model1.bin", settings.currModelFilename);

  EXPECT_EQ(RawSwitch(SWITCH_TYPE_TELEMETRY,1), settings.customFn[0].swtch);
  EXPECT_EQ(FuncLogs, settings.customFn[0].func);
  EXPECT_EQ(20, settings.customFn[0].param);

  EXPECT_EQ(RawSwitch(SWITCH_TYPE_ON), settings.customFn[1].swtch);
  EXPECT_EQ(FuncVolume, settings.customFn[1].func);
  EXPECT_EQ(RawSource(SOURCE_TYPE_STICK,4+5+1).toValue(), settings.customFn[1].param); // RS

  EXPECT_STREQ("Tes", settings.switchName[0]);
  EXPECT_EQ(Board::SWITCH_3POS, settings.switchConfig[0]);

  EXPECT_STREQ("BT_X10", settings.bluetoothName);
  EXPECT_STREQ("Default", settings.themeName);

#if !defined(USE_OTX)
  byteBuffer.clear();
  ASSERT_EQ(true, loadFile(byteBuffer, RADIO_TESTS_PATH "/model_22_x10/MODELS/model1.bin"));

  ModelData model;
  ASSERT_NE(nullptr, loadModelFromByteArray(model, byteBuffer));
#endif
  
  EXPECT_STREQ("Test", model.name);
  EXPECT_EQ(0, model.noGlobalFunctions);
  EXPECT_EQ(0, model.beepANACenter);
  EXPECT_EQ(80, model.mixData[0].weight);
  EXPECT_EQ(RawSource(SOURCE_TYPE_MAX), model.mixData[2].srcRaw); // MAX
  EXPECT_EQ(RawSource(SOURCE_TYPE_STICK, 4+5), model.mixData[3].srcRaw); // LS
  EXPECT_EQ(RawSource(SOURCE_TYPE_PPM, 0), model.mixData[5].srcRaw);
  EXPECT_EQ(RawSwitch(SWITCH_TYPE_TELEMETRY,1), model.mixData[5].swtch);
  EXPECT_EQ(900, model.limitData[0].max); // -100
  EXPECT_EQ(80, model.expoData[0].weight);
  EXPECT_EQ(LS_FN_VPOS, model.logicalSw[0].func);
  EXPECT_EQ(RawSource(SOURCE_TYPE_PPM,0).toValue(), model.logicalSw[0].val1); // TR1
  EXPECT_EQ(0, model.logicalSw[0].val2);
  EXPECT_EQ(RawSource(SOURCE_TYPE_TELEMETRY, 19*3).toValue(), model.logicalSw[1].val1); // TELE:20
  EXPECT_EQ(20, model.logicalSw[1].val2);
  EXPECT_EQ(RawSwitch(SWITCH_TYPE_VIRTUAL,1).toValue(), model.logicalSw[1].andsw);
  EXPECT_EQ(HELI_SWASH_TYPE_120X, model.swashRingData.type);
  EXPECT_STREQ("Tes", model.flightModeData[0].name);
  EXPECT_EQ(10, model.flightModeData[0].gvars[0]);
  EXPECT_STREQ("Tes", model.gvarData[0].name);
  EXPECT_EQ(PULSES_PXX_R9M, model.moduleData[1].protocol);
  EXPECT_EQ(MODULE_SUBTYPE_R9M_EU, model.moduleData[1].subType);
  EXPECT_STREQ("Rud", model.inputNames[0]);
  EXPECT_STREQ("Tes", model.sensorData[0].label);
  EXPECT_EQ(10, model.sensorData[0].id);
  EXPECT_EQ(9, model.sensorData[0].instance);
  EXPECT_EQ(5 + 2 + 3, model.thrTraceSrc); // CH3
}
