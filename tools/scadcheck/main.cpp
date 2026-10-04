// scadcheck: headless check of a .scad file against the editor pipeline.
// Parses the file, evaluates it with Manifold, regenerates OpenSCAD code and
// re-parses it, then prints triangle count, bounding box and volume for both.
//
//   scadcheck file.scad [--dump]

#include "hardwarelibrary.h"
#include "manifoldcsg.h"
#include "openscadgenerator.h"
#include "openscadparser.h"
#include "scenedocument.h"
#include "scenemesh.h"

#include <QCoreApplication>
#include <QFile>
#include <QTextStream>

#include <QFont>

#include <cstdio>
#include <limits>

// Font helper pulled in by scenetreetoolmetadata.cpp; the real one lives in
// scenetreecanvasgraphics.cpp, which drags in the whole graphics tree.
namespace SceneTreeGraphics {
QFont sceneTreeGraphicsFont() { return QFont(); }
}

static bool evaluate(const QString &code, const char *label, QString *generated)
{
    SceneDocument::Snapshot snapshot;
    QString error;
    int line = -1;
    if (!OpenScadParser::parseScene(code, &snapshot, &error, &line)) {
        std::printf("%s: PARSE ERROR line %d: %s\n", label, line, qPrintable(error));
        return false;
    }
    SceneDocument scene;
    scene.restoreSnapshot(snapshot);
    if (generated) *generated = OpenScadGenerator::generate(scene);

    SceneMesh mesh;
    if (!buildManifoldCsgMesh(scene, &mesh, &error)) {
        std::printf("%s: CSG FAILED: %s\n", label, qPrintable(error));
        return false;
    }

    QVector3D lo(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    QVector3D hi = -lo;
    double volume = 0.0;
    for (const MeshTriangle &t : mesh.triangles) {
        for (const QVector3D &p : {t.a, t.b, t.c}) {
            lo = QVector3D(qMin(lo.x(), p.x()), qMin(lo.y(), p.y()), qMin(lo.z(), p.z()));
            hi = QVector3D(qMax(hi.x(), p.x()), qMax(hi.y(), p.y()), qMax(hi.z(), p.z()));
        }
        volume += QVector3D::dotProduct(t.a, QVector3D::crossProduct(t.b, t.c)) / 6.0;
    }
    std::printf("%s: OK tris=%d bbox=[%.2f %.2f %.2f]..[%.2f %.2f %.2f] size=[%.2f %.2f %.2f] vol=%.1f\n",
                label, int(mesh.triangles.size()),
                lo.x(), lo.y(), lo.z(), hi.x(), hi.y(), hi.z(),
                hi.x() - lo.x(), hi.y() - lo.y(), hi.z() - lo.z(), volume);
    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    const QStringList args = app.arguments();
    if (args.size() < 2) {
        std::printf("usage: scadcheck file.scad [--dump]\n");
        return 2;
    }
    if (args.at(1) == QStringLiteral("--hardware")) {
        // Insert every hardware part at every size into an empty document,
        // one at a time, and evaluate the result.
        int failures = 0;
        for (const HardwareLibrary::Part &part : HardwareLibrary::parts()) {
            for (const HardwareLibrary::MetricSize &size : HardwareLibrary::metricSizes()) {
                const QString call = HardwareLibrary::callFor(part, size, size.defaultLength);
                const QString code = HardwareLibrary::insertPart(QString(), part.moduleName, call);
                const QByteArray label = QStringLiteral("%1 %2").arg(part.moduleName, size.name).toUtf8();
                QString generated;
                if (!evaluate(code, label.constData(), &generated)) ++failures;
            }
        }
        std::printf("hardware: %d failure(s)\n", failures);
        return failures == 0 ? 0 : 1;
    }

    QFile file(args.at(1));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::printf("cannot open %s\n", qPrintable(args.at(1)));
        return 2;
    }
    const QString code = QString::fromUtf8(file.readAll());

    QString generated;
    if (!evaluate(code, "original", &generated)) return 1;
    if (args.contains(QStringLiteral("--dump")))
        std::printf("----- generated -----\n%s\n---------------------\n", qPrintable(generated));
    QString regenerated;
    if (!evaluate(generated, "roundtrip", &regenerated)) return 1;
    if (regenerated != generated) {
        std::printf("roundtrip: generated code is NOT stable\n");
        return 1;
    }
    std::printf("roundtrip: stable\n");
    return 0;
}
