import java.awt.geom._
import java.awt.image._
import java.awt.image.AffineTransformOp._
import java.io.File
import javax.imageio.ImageIO
import org.apache.commons.io.FilenameUtils
import util.Random

import com.frugalmechanic.optparse._

import nebula._

object RunExperiments extends OptParse {
  def mkTable {
    val experiment = CorrespondenceExperiment(
      "",
      0,
      FASTDetector(100),
      SortExtractor(false, false, 8, 5, false),
      L0Matcher())

    val rowMutations: Seq[CorrespondenceExperiment => CorrespondenceExperiment] = 
      Seq(
	e => e.copy(otherImage = 4)
    )
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
    results.save

    println("Recognition rate was %s".format(Summary.recognitionRate(results.dmatches)))
  }

  val runtimeConfigFile = StrOpt()
  val experimentConfigFilesOrDirectory = MultiStrOpt()

  def main(args: Array[String]) { 
    parse(args)

    val runtimeConfig = IO.fromJSONFile[RuntimeConfig](new File(runtimeConfigFile.get))
    Global.runVar = Some(runtimeConfig)

    println(runtimeConfig)

    val experimentConfigFiles: List[File] = {
      val paths = experimentConfigFilesOrDirectory.get
      val files = paths.map(path => new File(path)).toList
      files match {
    	case List(file) => if (file.isDirectory) file.listFiles.toList
    			   else files
    	case _ => files
      }
    }

    val file = Global.homeDirectory + "/" + Global.run[RuntimeConfig].projectRoot + "/config/experiments/debug.json"

    if (experimentConfigFiles.size == 0) println("No experiments specified.")

    val experimentConfigs = experimentConfigFiles.map(CorrespondenceExperimentConfig.fromJSONFile)

    val experiments = experimentConfigs.flatMap(CorrespondenceExperiment.fromConfig)

    println("There are %s experiments:".format(experiments.size))
    experiments.foreach(println)

    experiments.foreach(runExperiment)

    println("All experiments completed.")
  }
}
