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
  def runExperiment(experiment: CorrespondenceExperiment) { 
    if (Global.run[RuntimeConfig].skipCompletedExperiments && experiment.alreadyRun) { 
      println("Already run, skipping: %s".format(experiment))
      return
    } else {
      println("Running experiment: %s".format(experiment))
    }

    println(experiment.leftImageFile)

    val leftImage = experiment.leftImage
    val rightImage = experiment.rightImage

    val (leftKeyPoints, rightKeyPoints) = {
      val leftKeyPoints = experiment.detector(leftImage)
      Util.pruneKeyPoints(leftImage, 
  			  rightImage, 
  			  experiment.homography, 
  			  leftKeyPoints).unzip
    }

    println("Number of KeyPoints: %s".format(leftKeyPoints.size))

    val (leftDescriptors, rightDescriptors) = {
      val leftDescriptors = experiment.extractor(leftImage, leftKeyPoints)
      val rightDescriptors = experiment.extractor(rightImage, rightKeyPoints)

      for ((Some(left), Some(right)) <- leftDescriptors.zip(rightDescriptors)) yield (left, right)
    } unzip

    println("Number of surviving KeyPoints: %s".format(leftDescriptors.size))

    val dmatches = experiment.matcher(true, leftDescriptors, rightDescriptors)

    val results = CorrespondenceExperimentResults(experiment, dmatches)
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
