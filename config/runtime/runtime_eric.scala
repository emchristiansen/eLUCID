import java.io.File

import nebula._

RuntimeConfig(
  projectRoot = new File(Global.homeDirectory, "week6/LUCID"),
  nebulaRoot = new File(Global.homeDirectory, "Dropbox/scala/12Summer/nebula"),
  parallel = false,
  tempDirectory = Some(new File("/tmp/correspondence")),
  deleteTemporaryFiles = false,
  skipCompletedExperiments = true,
  maxSimultaneousExperiments = 8)
