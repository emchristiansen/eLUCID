import java.awt.geom._
import java.awt.image._
import java.awt.image.AffineTransformOp._
import java.io.File
import javax.imageio.ImageIO
import org.apache.commons.io.FilenameUtils
import util.Random

import com.frugalmechanic.optparse._

import com.twitter.util._

import nebula._

object RunExperiments extends OptParse {
  def mkTable(experiments: Seq[Seq[CorrespondenceExperiment]]) {
    val table = Table(experiments)

    println("Table title is %s".format(table.title))
    println("Writing table to %s".format(table.path))
    org.apache.commons.io.FileUtils.writeStringToFile(table.path, table.toTSV)
  }

  def runExperiment(experiment: CorrespondenceExperiment) { 
    // TODO: Sort out the |skipCompletedExperiments| logic.
    if (Global.run[RuntimeConfig].skipCompletedExperiments && experiment.alreadyRun) { 
      println("Already run, skipping: %s".format(experiment))
      return
    } else {
      println("Running experiment: %s".format(experiment))
    }

    val results = CorrespondenceExperimentResults.runExperiment(experiment)

    println("Recognition rate was %s".format(Summary.recognitionRate(results.dmatches)))
  }

  def interpretFile[A](file: File): A = {
    val source = org.apache.commons.io.FileUtils.readFileToString(file)
    (new Eval).apply[A](source)
  }

  def init(runtimeConfigFile: File) {
    val runtimeConfig = interpretFile[RuntimeConfig](runtimeConfigFile)
    Global.runVar = Some(runtimeConfig)
    println(runtimeConfig)
  }

  val runtimeConfigFile = StrOpt()
  val experimentConfigFilesOrDirectory = MultiStrOpt()
  val tableConfigFiles = MultiStrOpt()

  def main(args: Array[String]) { 
    parse(args)

    init(new File(runtimeConfigFile.get))

    if (experimentConfigFilesOrDirectory.isDefined) {
      val experimentConfigFiles: Seq[File] = {
	val paths = experimentConfigFilesOrDirectory.get
	val files = paths.map(path => new File(path)).toSeq
	files match {
    	  case Seq(file) => if (file.isDirectory) file.listFiles.toList
    			    else files
    	  case _ => files
	}
      }

      val experiments = experimentConfigFiles.flatMap(interpretFile[Seq[CorrespondenceExperiment]])

      println("There are %s experiments:".format(experiments.size))
      experiments.foreach(println)

      Util.parallelize(experiments).foreach(runExperiment)

      println("All experiments completed.")
    }

    if (tableConfigFiles.isDefined) {
      val files = tableConfigFiles.get.map(name => new File(name))
      val tables = files.map(interpretFile[Seq[Seq[CorrespondenceExperiment]]])
      tables.foreach(mkTable)
    }
  }
}
