import java.awt.image._
import java.io.File
import javax.imageio.ImageIO
import org.apache.commons.io.FilenameUtils

import com.googlecode.javacv.cpp.opencv_contrib._
import com.googlecode.javacv.cpp.opencv_core._
import com.googlecode.javacv.cpp.opencv_features2d._


object Util { 
  def allCombinations[T](listList: List[List[T]]): List[List[T]] = {
    listList match {
      case head :: tail => for (h <- head; t <- allCombinations(tail)) yield h :: t
      case _ => List(List[T]())
    }
  }

  def runSystemCommand(command: String): String = {
    println("running system command: %s".format(command))
    try {
      val out = sys.process.Process(command).!!
      println("successfully ran system command")
      out
    } catch {
      case e: Exception => throw new Exception("system command failed: %s\nException was %s\n".format(command, e.toString))
    } 
  }

  def pruneKeyPoints(
    leftImage: BufferedImage, 
    rightImage: BufferedImage, 
    homography: Homography, 
    leftKeyPoints: List[KeyPoint]): List[Tuple2[KeyPoint, KeyPoint]] = {
    val leftWidth = leftImage.getWidth
    val leftHeight = leftImage.getHeight
    val insideLeft = leftKeyPoints.filter(KeyPoint.isWithinBounds(leftWidth, leftHeight))

    val rightKeyPoints = insideLeft.map(homography.transform)

    val rightWidth = rightImage.getWidth
    val rightHeight = rightImage.getHeight
    for ((left, right) <- leftKeyPoints.zip(rightKeyPoints)
         if KeyPoint.isWithinBounds(rightWidth, rightHeight)(right)) yield (left, right) 
  }

  def imageWidthAndHeight(path: String): Tuple2[Int, Int] = {
    val command = "identify %s".format(path)

    val stdout = runSystemCommand(command)
    val Parser = """(\d+)x(\d+)""".r
    val Parser(width, height) = stdout.split(" ")(2)
    (width.toInt, height.toInt)
  }

  def linesFromFile(file: File): List[String] = {
    io.Source.fromFile(file).mkString.split("\n").map(_.trim).filter(_.size > 0).filter(_.take(2) != "//").toList
  }

  def assertWithValue[A](assertion: () => Boolean, value: A): A = {
    assert(assertion())
    value
  }

  def recursiveListFiles(f: File): List[File] = { 
    if (!f.exists) throw new Exception("path does not exists: %s".format(f.toString))

    val these = f.listFiles.toList
    these ++ these.filter(_.isDirectory).flatMap(recursiveListFiles)
  }

  def createTempFile(prefix: String, suffix: String) = {
    val file = File.createTempFile(prefix, suffix)
    if (Global.run.deleteTemporaryFiles) file.deleteOnExit
    file
  }

  def truncate(list: List[Double]): String = { 
    list.map(l => "%.4f".format(l)).mkString(" ")
  }
}
