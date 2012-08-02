import org.apache.commons.math3.linear._
import java.io.File
import com.googlecode.javacv.cpp.opencv_features2d._

case class Homography(matrix: RealMatrix) {
  def transform(keyPoint: KeyPoint): KeyPoint = {
    val homogeneousIn = new ArrayRealVector(Array(keyPoint.pt_x.toDouble, 
						  keyPoint.pt_y.toDouble, 
						  1))
    val homogeneousOut = {
      val unscaled = matrix.operate(homogeneousIn)
      unscaled.mapMultiply(1.0 / unscaled.getEntry(2))
    }

    val outX = homogeneousOut.getEntry(0).toFloat
    val outY = homogeneousOut.getEntry(1).toFloat

    // TODO: Map over components necessary for invariant descriptors, such as
    // size and angle.
    KeyPoint.withDefaults(outX, outY)
  }
}

object Homography {
  def fromFile(file: File): Homography = {
    val lines = Util.linesFromFile(file)
    val values = lines.map(_.split("[ \t]").filter(_.size > 0).map(_.toDouble).toArray).toArray
    Homography(new Array2DRowRealMatrix(values))
  }
}
