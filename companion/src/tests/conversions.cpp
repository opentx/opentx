#include "gtests.h"
#include "location.h"
#include "storage/storage.h"

TEST(Conversions, test_conversion)
{
  QString   error;
  RadioData simuData;
  Storage   store = Storage(TESTS_PATH "/eeprom_22_x9d+.bin");

  EXPECT_EQ(true, store.load(simuData));
  qDebug() << store.error();
}
