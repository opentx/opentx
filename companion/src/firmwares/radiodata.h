#ifndef _RADIODATA_H_
#define _RADIODATA_H_

#include "generalsettings.h"
#include "modeldata.h"

#include <QtCore>

class RadioDataConversionState;

class CategoryData {
  public:
    CategoryData(const char * name) {
      strncpy(this->name, name, sizeof(CategoryData::name));
    }
    char name[15+1];
};

class RadioData {
  Q_DECLARE_TR_FUNCTIONS(RadioData)

  public:
    RadioData();

    GeneralSettings generalSettings;
    std::vector<CategoryData> categories;
    std::vector<ModelData> models;

    void convert(RadioDataConversionState & cstate);

    void setCurrentModel(unsigned int index);
    void fixModelFilenames();
    QString getNextModelFilename();

    static QString getElementName(const QString & prefix, unsigned int index, const char * name = 0, bool padding = false);

  protected:
    void fixModelFilename(unsigned int index);
};

#endif // _RADIODATA_H_
