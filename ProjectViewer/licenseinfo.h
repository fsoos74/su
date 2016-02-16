#ifndef LICENSEINFO_H
#define LICENSEINFO_H

namespace license{

const int MajorNumberAddress=0;
const int MinorNumberAddress=1;
const int ProductVersionAddress=10;

struct LicenseInfo{
  int dongleSerialNumber;
  int majorNumber;
  int minorNumber;
  int productVersion;
  int expirationDay;
  int expirationMonth;
  int expirationYear;
};

}

#endif // LICENSEINFO_H
