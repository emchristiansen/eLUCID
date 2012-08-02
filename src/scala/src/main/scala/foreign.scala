import java.io.File

import com.googlecode.javacv.cpp.opencv_contrib._
import com.googlecode.javacv.cpp.opencv_core._
import com.googlecode.javacv.cpp.opencv_features2d._

case class Descriptor()

object KeyPoint {
  val defaultSize = 0
  val defaultAngle = -1
  val defaultResponse = 0
  val defaultOctave = 0
  val defaultClassID = -1

  def withDefaults(x: Float, y: Float): KeyPoint = {
    new KeyPoint(x, y, defaultSize, defaultAngle, defaultResponse, defaultOctave, defaultClassID)
  }

  def isWithinBounds(width: Int, height: Int)(keyPoint: KeyPoint): Boolean = {
    def linearBound(max: Int, pt: Double): Boolean = (pt >= 0) && (pt < max)
    linearBound(width, keyPoint.pt_x) && linearBound(height, keyPoint.pt_y)
  }
}
