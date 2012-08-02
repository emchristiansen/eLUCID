import java.awt.image._
import java.io.File
import xml._
import XMLUtil._

import com.googlecode.javacv.cpp.opencv_contrib._
import com.googlecode.javacv.cpp.opencv_core._
import com.googlecode.javacv.cpp.opencv_features2d._


sealed trait CorrespondenceMethod {
  val abbreviation: String
}

object CorrespondenceMethod {
  def fromConstructorsAndXMLOption[A](constructors: List[FromXML[A]])
				     (node: Node): Option[A] = {
    // Using monadic properties of flatten.
    val matches = constructors.map(_.fromXMLOption(node)).flatten
    if (matches.size == 1) Some(matches.head)
    else None
  }

  def fromMethodAndXMLOption[A](method: String, 
				constructor: Node => A) 
                               (node: Node): Option[A] = {
    if (singleOnKey(_.text, "method")(node) == method) Some(constructor(node))
    else None
  }
}

// -------------------------------------------------------------------------- //

sealed trait DetectorMethod extends CorrespondenceMethod {
  val maxKeyPoints: Int
  def apply(image: BufferedImage): List[KeyPoint]
}

object DetectorMethod extends FromXML[DetectorMethod] {
  // TODO: How do I use point-free style for this method?
  def fromXMLOption(node: Node) = 
    CorrespondenceMethod.fromConstructorsAndXMLOption(List(FASTDetector))(node)
}

case class FASTDetector(val maxKeyPoints: Int) extends DetectorMethod {
  val abbreviation = "FAST-MKP-%s".format(maxKeyPoints)

  def apply(image: BufferedImage): List[KeyPoint] = {
    // TODO
    throw new Exception
  }
}

object FASTDetector extends FromXML[FASTDetector] {
  def constructor(node: Node): FASTDetector = {
    // TODO: How to do this elegantly?
    implicit val implicitNode = node
    FASTDetector(singleOnKey(_.text.toInt, "maxKeyPoints"))
  }

  def fromXMLOption(node: Node) = 
    CorrespondenceMethod.fromMethodAndXMLOption("FAST", constructor _)(node)
}

// -------------------------------------------------------------------------- //

sealed trait ExtractorMethod extends CorrespondenceMethod {
  def apply(keyPoints: List[KeyPoint], image: BufferedImage): List[Option[Descriptor]]
}

object ExtractorMethod extends FromXML[ExtractorMethod] {
  def fromXMLOption(node: Node) =
    CorrespondenceMethod.fromConstructorsAndXMLOption(List(SortExtractor))(node)
}

case class SortExtractor(val normalizeRotation: Boolean,
			 val normalizeScale: Boolean,
			 val patchWidth: Int,
			 val blurWidth: Int,
			 val color: Boolean) extends ExtractorMethod {
  // TODO: This is pretty cool, but can I abstract this into the superclass?
//  val parameters = SortExtractor.unapply(this).get
  val parameterStrings = this.productIterator.toList.map(_.toString)
  val abbreviation = "Sort-NR-%s-NS-%s-PW-%s-BW-%s-C-%s".format(parameterStrings: _*)

  def apply(keyPoints: List[KeyPoint], 
	    image: BufferedImage): List[Option[Descriptor]] = {
    // TODO
    throw new Exception
  }	    
}

object SortExtractor extends FromXML[SortExtractor] {
  def constructor(node: Node): SortExtractor = {
    implicit val implicitNode = node
    SortExtractor(singleOnKey(_.text.toBoolean, "normalizeRotation"),
		  singleOnKey(_.text.toBoolean, "normalizeScale"),
		  singleOnKey(_.text.toInt, "patchWidth"),
		  singleOnKey(_.text.toInt, "blurWidth"),
		  singleOnKey(_.text.toBoolean, "color"))
  }
 
  def fromXMLOption(node: Node) =
    CorrespondenceMethod.fromMethodAndXMLOption("Sort", constructor _)(node)
}

// -------------------------------------------------------------------------- //

sealed trait MatcherMethod extends CorrespondenceMethod {
  def apply(allPairs: Boolean, 
	    leftDescriptors: List[Descriptor], 
	    rightDescriptors: List[Descriptor]): List[DMatch]
}

object MatcherMethod extends FromXML[MatcherMethod] {
  def fromXMLOption(node: Node) =
    CorrespondenceMethod.fromConstructorsAndXMLOption(List(KendallTauMatcher))(node)
}

case class KendallTauMatcher() extends MatcherMethod {
  val abbreviation = "KT"

  def apply(allPairs: Boolean,
	    leftDescriptors: List[Descriptor],
	    rightDescriptors: List[Descriptor]): List[DMatch] = {
    throw new Exception
  }
}

object KendallTauMatcher extends FromXML[KendallTauMatcher] {
  def constructor(node: Node): KendallTauMatcher = KendallTauMatcher()

  def fromXMLOption(node: Node) = 
    CorrespondenceMethod.fromMethodAndXMLOption("KendallTau", constructor _)(node)
}
