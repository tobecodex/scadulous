#include "App.h"
#include <QSurfaceFormat>

int main(int argc, char **argv)
{
  QSurfaceFormat format;
  format.setDepthBufferSize(24);
  format.setMajorVersion(4);
  format.setMinorVersion(3);
  format.setSamples(4);
  format.setProfile(QSurfaceFormat::CompatibilityProfile);
  format.setOption(QSurfaceFormat::DebugContext);
  QSurfaceFormat::setDefaultFormat(format);

  App app(argc, argv);
  return app.exec();
}
