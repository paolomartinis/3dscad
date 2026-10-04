#ifndef HARDWARELIBRARY_H
#define HARDWARELIBRARY_H

#include <QString>
#include <QVector>

// Metric fasteners and holes from hardware/hardware.scad (embedded as a Qt
// resource). Each part maps an ISO size to a module call; insertPart() copies
// the module (and the helper modules it uses) into a document and appends
// the call.
namespace HardwareLibrary {

// ISO dimensions for one metric size, in mm.
struct MetricSize
{
    QString name;          // "M3"
    double d;              // nominal diameter
    double pitch;          // coarse pitch
    double hexS;           // hex head / nut width across flats (ISO 4017 / 4032)
    double hexK;           // hex head height (ISO 4017)
    double nutM;           // hex nut height (ISO 4032)
    double nylocH;         // nyloc nut overall height (ISO 10511)
    double socketDk;       // socket head diameter (ISO 4762); head height = d
    double socketKey;      // socket head hex key (ISO 4762)
    double cskDk;          // countersunk head diameter (ISO 10642)
    double cskK;           // countersunk head height (ISO 10642)
    double cskKey;         // countersunk hex key (ISO 10642)
    double washerD1;       // washer inner diameter (ISO 7089)
    double washerD2;       // washer outer diameter (ISO 7089)
    double washerH;        // washer thickness (ISO 7089)
    double clearanceD;     // clearance hole, ISO 273 medium
    double counterboreD;   // counterbore diameter for socket heads
    double insertD;        // heat-set insert pilot hole diameter (typical)
    double insertDepth;    // heat-set insert pilot hole depth (typical)
    double defaultLength;  // suggested screw length
    double buttonDk;       // button head diameter (ISO 7380)
    double buttonK;        // button head height (ISO 7380)
    double buttonKey;      // button head hex key (ISO 7380)
    double grubKey;        // set screw hex key (ISO 4026)
};

enum class LengthKind { None, ScrewLength, RodLength, HoleDepth };

struct Part
{
    QString category;      // menu group: "Screws", "Nuts", ...
    QString label;         // menu entry
    QString moduleName;
    LengthKind lengthKind;
};

const QVector<MetricSize> &metricSizes();
const QVector<Part> &parts();

// OpenSCAD call for a part at a size; length is ignored for LengthKind::None.
QString callFor(const Part &part, const MetricSize &size, double length);

// Returns code with the part's module definitions (when missing) inserted
// after the leading comment block and the call appended at the end.
QString insertPart(const QString &code, const QString &moduleName, const QString &call);

} // namespace HardwareLibrary

#endif // HARDWARELIBRARY_H
