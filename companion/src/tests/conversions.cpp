#include "gtests.h"
#include "location.h"
#include "storage/storage.h"
#include "firmwares/opentx/opentxinterface.h"

TEST(Conversions, ConversionX9DPFrom22)
{
  RadioData radioData;
  Storage   store = Storage(RADIO_TESTS_PATH "/eeprom_22_x9d+.bin");

  ASSERT_EQ(true, store.load(radioData));
  
  const ModelData& model = radioData.models[0];
  EXPECT_STREQ("Test", model.name);
  EXPECT_EQ(RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_SA0), model.timers[0].mode);
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

TEST(Conversions, ConversionHorusFrom22)
{
  QByteArray modelByteArray;
  ASSERT_EQ(true, loadFile(modelByteArray, RADIO_TESTS_PATH "/MODELS/model_22_horus.bin"));

  ModelData model;
  ASSERT_NE(nullptr, loadModelFromByteArray(model, modelByteArray));
  
  EXPECT_STREQ("Test", model.name);
  EXPECT_EQ(80, model.mixData[0].weight);
  EXPECT_EQ(RawSource(SOURCE_TYPE_PPM,0), model.mixData[4].srcRaw);
  EXPECT_EQ(RawSwitch(SWITCH_TYPE_TELEMETRY,1), model.mixData[4].swtch);
  EXPECT_EQ(900, model.limitData[0].max); // -100
  EXPECT_EQ(80, model.expoData[0].weight);
  EXPECT_EQ(LS_FN_VPOS, model.logicalSw[0].func);
  EXPECT_EQ(RawSource(SOURCE_TYPE_PPM,0).toValue(), model.logicalSw[0].val1);
  EXPECT_EQ(0, model.logicalSw[0].val2);
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
}
