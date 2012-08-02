import java.awt.image._
import java.io.File
import javax.imageio.ImageIO
import xml._

case class ExperimentConfig(
  val imageClasses: List[String],
  val otherImages: List[Int],
  val detectors: List[DetectorMethod],
  val extractors: List[ExtractorMethod],
  val matchers: List[MatcherMethod])

object ExperimentConfig extends FromXML[ExperimentConfig] { 
  def fromXMLOption(node: Node): Option[ExperimentConfig] = {
    import XMLUtil._
    implicit val implicitNode = node
    // TODO, make sure xml is <experimentConfig>
    val config = ExperimentConfig(mapOnKey(_.text, "imageClasses"),
				  mapOnKey(_.text.toInt, "otherImages"),
				  mapOnKey(DetectorMethod.fromXML, "detector"),
				  mapOnKey(ExtractorMethod.fromXML, "descriptor"),
				  mapOnKey(MatcherMethod.fromXML, "matcher"))
    Some(config)
  }
}

case class RuntimeConfig(
  val projectRoot: File,
  val deleteTemporaryFiles: Boolean,
  val skipCompletedExperiments: Boolean
) {
  def childPath(child: String): File = {
    val path = new File(projectRoot, child)
    if (!path.exists) throw new Exception("Path does not exist: %s".format(path))
    path
  }
}
  
object RuntimeConfig extends FromXML[RuntimeConfig] { 
  def fromXMLOption(node: Node): Option[RuntimeConfig] = {
    import XMLUtil._
    implicit val implicitNode = node
    val config = 
      RuntimeConfig(singleOnKey(path => new File(Global.homeDirectory, path.text), 
				"projectRoot"),
		    singleOnKey(_.text.toBoolean, "deleteTemporaryFiles"),
		    singleOnKey(_.text.toBoolean, "skipCompletedExperiments"))
    Some(config)
  }
}

case class Experiment(
  val imageClass: String,
  val otherImage: Int,
  val detector: DetectorMethod,
  val extractor: ExtractorMethod,
  val matcher: MatcherMethod) {
  val filenameNoTime: String = {
    val parameterAbbreviations: List[String] = "IC OI D E M".split(" ").toList
    val parameterValues: List[String] = List(imageClass, 
					     otherImage.toString, 
					     detector.abbreviation, 
					     extractor.abbreviation, 
					     matcher.abbreviation)
    val parts = (parameterAbbreviations, parameterValues).zipped.map(_ + "-" + _)
    parts.mkString("_") + ".xml"
  }

  val unixEpoch = System.currentTimeMillis / 1000L
  val filename: String = unixEpoch + "_" + filenameNoTime

  lazy val outDirectory: File = Global.run.childPath("results/experiment_data")

  lazy val path: File = {
    new File(outDirectory, filename)
  }

  def existingResultsFiles: List[File] = {
    val allPaths = outDirectory.list.toList.map(_.toString)
    val matchingPaths = allPaths.filter(_.contains(filenameNoTime))
    matchingPaths.sortBy(identity).reverse.map(path => new File(path))
  }

  def existingResultsFile: Option[File] = {
    existingResultsFiles match {
      case Nil => None
      case file :: _ => Some(file)
    }
  }

  def alreadyRun: Boolean = !existingResultsFile.isEmpty

  lazy val leftImageFile = Global.run.childPath("data/%s/images/img1.ppm".format(imageClass))
  def leftImage = ImageIO.read(leftImageFile)
  lazy val rightImageFile = Global.run.childPath("data/%s/images/img%s.ppm".format(imageClass, otherImage))
  def rightImage = ImageIO.read(rightImageFile)
  lazy val homographyFile = Global.run.childPath("data/%s/homographies/H1to%sp".format(imageClass, otherImage))
  def homography = Homography.fromFile(homographyFile)
}

object Experiment{
  def fromConfig(config: ExperimentConfig): List[Experiment] = {
    for (ic <- config.imageClasses;
	 oi <- config.otherImages;
	 d <- config.detectors;
	 e <- config.extractors;
	 m <- config.matchers) yield {
      Experiment(ic, oi, d, e, m)
    }
  }
}
