#include "hardwarelibrary.h"

#include <QFile>
#include <QHash>
#include <QRegularExpression>
#include <QStringList>

namespace HardwareLibrary {

const QVector<MetricSize> &metricSizes()
{
    // name   d    pitch  hexS hexK  nutM  nyloc sockDk key  cskDk  cskK  cskKey w.d1  w.d2 w.h  clr  cbore insD insL  len  btnDk btnK btnKey grubKey
    static const QVector<MetricSize> sizes = {
        {"M2",   2,   0.4,  4,   1.4,  1.6,  3.0,  3.8,  1.5, 3.8,   1.2,  1.3,  2.2,  5,   0.3, 2.4,  4.4, 3.2, 4.0,  8, 3.5,  1.3,  1.3, 0.9},
        {"M2.5", 2.5, 0.45, 5,   1.7,  2.0,  3.5,  4.5,  2,   4.7,   1.5,  1.5,  2.7,  6,   0.5, 2.9,  5.5, 3.5, 5.0,  10, 4.7,  1.5,  1.5, 1.3},
        {"M3",   3,   0.5,  5.5, 2,    2.4,  4,    5.5,  2.5, 6.72,  1.86, 2,    3.2,  7,   0.5, 3.4,  6.5, 4.0, 5.7,  10, 5.7,  1.65, 2,   1.5},
        {"M4",   4,   0.7,  7,   2.8,  3.2,  5,    7,    3,   8.96,  2.48, 2.5,  4.3,  9,   0.8, 4.5,  8,   5.6, 8.1,  12, 7.6,  2.2,  2.5, 2},
        {"M5",   5,   0.8,  8,   3.5,  4.7,  5,    8.5,  4,   11.2,  3.1,  3,    5.3,  10,  1,   5.5,  10,  6.4, 9.5,  16, 9.5,  2.75, 3,   2.5},
        {"M6",   6,   1,    10,  4,    5.2,  6,    10,   5,   13.44, 3.72, 4,    6.4,  12,  1.6, 6.6,  11,  8.0, 12.7, 20, 10.5, 3.3,  4,   3},
        {"M8",   8,   1.25, 13,  5.3,  6.8,  8,    13,   6,   17.92, 4.96, 5,    8.4,  16,  1.6, 9,    15,  10,  13,   25, 14,   4.4,  5,   4},
        {"M10",  10,  1.5,  16,  6.4,  8.4,  10,   16,   8,   22.4,  6.2,  6,    10.5, 20,  2,   11,   18,  12.5, 15,  30, 17.5, 5.5,  6,   5},
        {"M12",  12,  1.75, 18,  7.5,  10.8, 12,   18,   10,  26.88, 7.44, 8,    13,   24,  2.5, 13.5, 20,  15,  18,   40, 21,   6.6,  8,   6},
    };
    return sizes;
}

const QVector<Part> &parts()
{
    static const QVector<Part> list = {
        {"Screws",                   "Hex bolt (ISO 4017)",            "hex_bolt",                   LengthKind::ScrewLength},
        {"Screws",                   "Socket head (ISO 4762)",         "socket_head_screw",          LengthKind::ScrewLength},
        {"Screws",                   "Countersunk (ISO 10642)",        "countersunk_screw",          LengthKind::ScrewLength},
        {"Screws",                   "Button head (ISO 7380)",         "button_head_screw",          LengthKind::ScrewLength},
        {"Screws",                   "Set screw / grub (ISO 4026)",    "set_screw",                  LengthKind::ScrewLength},
        {"Screws",                   "Threaded rod",                   "threaded_rod",               LengthKind::RodLength},
        {"Screws (printable thread)", "Hex bolt (ISO 4017)",           "hex_bolt_threaded",          LengthKind::ScrewLength},
        {"Screws (printable thread)", "Socket head (ISO 4762)",        "socket_head_screw_threaded", LengthKind::ScrewLength},
        {"Screws (printable thread)", "Countersunk (ISO 10642)",       "countersunk_screw_threaded", LengthKind::ScrewLength},
        {"Screws (printable thread)", "Button head (ISO 7380)",        "button_head_screw_threaded", LengthKind::ScrewLength},
        {"Screws (printable thread)", "Set screw / grub (ISO 4026)",   "set_screw_threaded",         LengthKind::ScrewLength},
        {"Screws (printable thread)", "Threaded rod",                  "threaded_rod_threaded",      LengthKind::RodLength},
        {"Nuts && Washers",          "Hex nut (ISO 4032)",             "hex_nut",                    LengthKind::None},
        {"Nuts && Washers",          "Nyloc nut (ISO 10511)",          "nyloc_nut",                  LengthKind::None},
        {"Nuts && Washers",          "Washer (ISO 7089)",              "washer",                     LengthKind::None},
        {"Holes (use as cuts)",      "Clearance hole",                 "clearance_hole",             LengthKind::HoleDepth},
        {"Holes (use as cuts)",      "Counterbore (socket head)",      "counterbore_hole",           LengthKind::HoleDepth},
        {"Holes (use as cuts)",      "Countersink",                    "countersink_hole",           LengthKind::HoleDepth},
        {"Holes (use as cuts)",      "Nut trap",                       "nut_trap",                   LengthKind::HoleDepth},
        {"Holes (use as cuts)",      "Heat-set insert",                "heat_insert_hole",           LengthKind::None},
        {"Holes (use as cuts)",      "Tapped hole (printable thread)", "tapped_hole",                LengthKind::HoleDepth},
    };
    return list;
}

static QString num(double value)
{
    return QString::number(value, 'g', 6);
}

QString callFor(const Part &part, const MetricSize &s, double length)
{
    const QString l = num(length);
    const QString &m = part.moduleName;
    QStringList args;
    auto arg = [&args](const char *name, const QString &value) {
        args << QStringLiteral("%1 = %2").arg(QLatin1String(name), value);
    };

    if (m.startsWith("hex_bolt")) {
        arg("d", num(s.d)); arg("l", l); arg("s", num(s.hexS)); arg("k", num(s.hexK));
    } else if (m.startsWith("socket_head_screw")) {
        arg("d", num(s.d)); arg("l", l); arg("dk", num(s.socketDk)); arg("k", num(s.d));
        arg("key", num(s.socketKey));
    } else if (m.startsWith("countersunk_screw")) {
        arg("d", num(s.d)); arg("l", l); arg("dk", num(s.cskDk)); arg("k", num(s.cskK));
        arg("key", num(s.cskKey));
    } else if (m.startsWith("button_head_screw")) {
        arg("d", num(s.d)); arg("l", l); arg("dk", num(s.buttonDk)); arg("k", num(s.buttonK));
        arg("key", num(s.buttonKey));
    } else if (m.startsWith("set_screw")) {
        arg("d", num(s.d)); arg("l", l); arg("key", num(s.grubKey));
    } else if (m.startsWith("threaded_rod")) {
        arg("d", num(s.d)); arg("l", l);
    } else if (m == "hex_nut") {
        arg("d", num(s.d)); arg("s", num(s.hexS)); arg("m", num(s.nutM));
    } else if (m == "nyloc_nut") {
        arg("d", num(s.d)); arg("s", num(s.hexS)); arg("m", num(s.nutM)); arg("h", num(s.nylocH));
    } else if (m == "washer") {
        arg("d1", num(s.washerD1)); arg("d2", num(s.washerD2)); arg("h", num(s.washerH));
    } else if (m == "clearance_hole") {
        arg("d", num(s.clearanceD)); arg("l", l);
    } else if (m == "counterbore_hole") {
        arg("d", num(s.clearanceD)); arg("l", l); arg("cb_d", num(s.counterboreD));
        arg("cb_h", num(s.d + 0.4));
    } else if (m == "countersink_hole") {
        arg("d", num(s.clearanceD)); arg("l", l); arg("dk", num(s.cskDk + 0.4));
        arg("k", num(s.cskK + 0.2));
    } else if (m == "nut_trap") {
        // Printing clearance on the pocket so a metal nut presses in.
        arg("d", num(s.clearanceD)); arg("s", num(s.hexS + 0.4)); arg("m", num(s.nutM + 0.4));
        arg("l", l);
    } else if (m == "heat_insert_hole") {
        arg("d", num(s.insertD)); arg("depth", num(s.insertDepth));
    } else if (m == "tapped_hole") {
        arg("d", num(s.d)); arg("l", l);
    }

    if (m.endsWith("_threaded") || m == "tapped_hole")
        arg("pitch", num(s.pitch));
    if (m == "tapped_hole")
        arg("clearance", "0.2");

    return QStringLiteral("%1(%2);").arg(m, args.join(QStringLiteral(", ")));
}

// Module name -> full "module name(...) { ... }" text from the embedded library.
static const QHash<QString, QString> &libraryModules()
{
    static const QHash<QString, QString> modules = []() {
        QHash<QString, QString> result;
        QFile file(QStringLiteral(":/hardware/hardware.scad"));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return result;
        const QString text = QString::fromUtf8(file.readAll());
        static const QRegularExpression header(QStringLiteral("^module\\s+(\\w+)\\s*\\("),
                                               QRegularExpression::MultilineOption);
        auto it = header.globalMatch(text);
        while (it.hasNext()) {
            const QRegularExpressionMatch match = it.next();
            const int open = text.indexOf(QLatin1Char('{'), match.capturedEnd());
            if (open < 0) break;
            int depth = 0;
            int close = -1;
            for (int i = open; i < text.size(); ++i) {
                if (text.at(i) == QLatin1Char('{')) ++depth;
                else if (text.at(i) == QLatin1Char('}') && --depth == 0) { close = i; break; }
            }
            if (close < 0) break;
            result.insert(match.captured(1), text.mid(match.capturedStart(), close - match.capturedStart() + 1));
        }
        return result;
    }();
    return modules;
}

// Appends moduleName and every library module it calls, dependencies first.
static void collectModules(const QString &moduleName, QStringList *ordered)
{
    if (ordered->contains(moduleName)) return;
    const QString body = libraryModules().value(moduleName);
    if (body.isEmpty()) return;
    for (auto it = libraryModules().cbegin(); it != libraryModules().cend(); ++it) {
        if (it.key() == moduleName) continue;
        const QRegularExpression call(QStringLiteral("\\b%1\\s*\\(").arg(it.key()));
        if (body.indexOf(call, body.indexOf(QLatin1Char('{'))) >= 0)
            collectModules(it.key(), ordered);
    }
    ordered->append(moduleName);
}

QString insertPart(const QString &code, const QString &moduleName, const QString &call)
{
    QStringList needed;
    collectModules(moduleName, &needed);

    QString definitions;
    for (const QString &name : needed) {
        const QRegularExpression declared(QStringLiteral("^\\s*module\\s+%1\\s*\\(").arg(name),
                                          QRegularExpression::MultilineOption);
        if (!code.contains(declared))
            definitions += libraryModules().value(name) + QStringLiteral("\n\n");
    }

    // Keep the leading comment block (generator header) on top.
    const QStringList lines = code.split(QLatin1Char('\n'));
    int headerLines = 0;
    while (headerLines < lines.size()
           && (lines.at(headerLines).trimmed().isEmpty()
               || lines.at(headerLines).trimmed().startsWith(QStringLiteral("//"))))
        ++headerLines;
    const QString header = lines.mid(0, headerLines).join(QLatin1Char('\n'));
    QString body = lines.mid(headerLines).join(QLatin1Char('\n'));
    while (body.endsWith(QLatin1Char('\n'))) body.chop(1);

    QString result = header;
    if (!result.isEmpty()) result += QLatin1Char('\n');
    result += definitions + body;
    if (!body.isEmpty()) result += QLatin1Char('\n');
    result += call + QLatin1Char('\n');
    return result;
}

} // namespace HardwareLibrary
