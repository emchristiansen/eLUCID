import java.io.File
import xml._

trait FromXML[+A] {
  def fromXMLOption(node: Node): Option[A]
  def fromXML(node: Node): A = fromXMLOption(node).get
  def fromFile(file: File): A = fromXML(XML.loadFile(file))
}

object XMLUtil {
  def mapOnKey[A](function: (Node) => A, key: String)
                 (implicit node: Node): List[A] =
    (node \ key).toList.map(function)

  def singleOnKey[A](function: (Node) => A, key: String)
                    (implicit node: Node): A = {
    val list = mapOnKey(function, key)
    assert(list.size == 1)
    list.head
  }
}
