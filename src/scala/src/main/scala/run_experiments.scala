import java.awt.geom._
import java.awt.image._
import java.awt.image.AffineTransformOp._
import java.io.File
import javax.imageio.ImageIO
import org.apache.commons.io.FilenameUtils
import util.Random

import com.frugalmechanic.optparse._

object Global {
  val homeDirectory = System.getProperty("user.home")
  var runVar: Option[RuntimeConfig] = None
  def run: RuntimeConfig = runVar match {
    case Some(config) => config
    case None => throw new Exception("Global.run not initialized")
  }
}

object RunExperiments extends OptParse {
  def runExperiment(experiment: Experiment) { 
    if (Global.run.skipCompletedExperiments && experiment.alreadyRun) { 
      println("Already run, skipping: %s".format(experiment))
      return
    } else {
      println("Running experiment: %s".format(experiment))
    }

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
      val leftDescriptors = experiment.extractor(leftKeyPoints, leftImage)
      val rightDescriptors = experiment.extractor(rightKeyPoints, rightImage)

      for ((Some(left), Some(right)) <- leftDescriptors.zip(rightDescriptors)) yield (left, right)
    } unzip

    println("Number of surviving KeyPoints: %s".format(leftDescriptors.size))

    val dmatches = experiment.matcher(true, leftDescriptors, rightDescriptors)

    // TODO: save ExperimentResults and print recognition rate
  }

  val runtimeConfigFile = StrOpt()
  val experimentConfigFilesOrDirectory = MultiStrOpt()
  def main(args: Array[String]) { 
    parse(args)

    val runtimeConfig = RuntimeConfig.fromFile(new File(runtimeConfigFile.get))
    Global.runVar = Some(runtimeConfig)

    val experimentConfigFiles: List[File] = {
      val paths = experimentConfigFilesOrDirectory.get
      val files = paths.map(path => new File(path)).toList
      files match {
	case List(file) => if (file.isDirectory) file.listFiles.toList
			   else files
	case _ => files
      }
    }

    if (experimentConfigFiles.size == 0) println("No experiments specified.")
    
    val experimentConfigs = experimentConfigFiles.map(ExperimentConfig.fromFile)
    val experiments = experimentConfigs.flatMap(Experiment.fromConfig)

    println("There are %s experiments:".format(experiments.size))
    experiments.foreach(println)

    experiments.foreach(runExperiment)

    println("All experiments completed.")
  }
}
