import sbt._
import Keys._

import sbtassembly.Plugin._
import AssemblyKeys._

object SFSPipelineBuild extends Build {
  def extraResolvers = Seq(
    resolvers ++= Seq(
      "NativeLibs4Java Respository" at "http://nativelibs4java.sourceforge.net/maven/",
      "Sonatype OSS Snapshots Repository" at "http://oss.sonatype.org/content/groups/public",
      "repo.codahale.com" at "http://repo.codahale.com",
      "maven.twttr.com" at "http://maven.twttr.com"
    )
  )

  def extraLibraryDependencies = Seq(
    libraryDependencies ++= Seq(
      "commons-lang" % "commons-lang" % "2.6",
      "org.apache.commons" % "commons-math3" % "3.0",
      "org.apache.commons" % "commons-math" % "2.2",
      "commons-io" % "commons-io" % "2.3",
      "com.frugalmechanic" % "scala-optparse" % "1.0",
      "nebula" %% "nebula" % "0.1-SNAPSHOT",
      "org.clapper" %% "grizzled-scala" % "1.0.13",
      "com.twitter" % "util-eval" % "5.3.6"
    )
  )

  def scalaSettings = Seq(
    scalaVersion := "2.9.2",
    scalacOptions ++= Seq(
      "-optimize",
      "-unchecked",
      "-deprecation"
    )
  )

  def libSettings = Project.defaultSettings ++ extraResolvers ++ extraLibraryDependencies ++ scalaSettings ++ assemblySettings

  lazy val root = {
    val longName = "lucid_test"
    val settings = libSettings ++ Seq(name := longName)
    Project(id = longName, base = file("."), settings = settings)
  }
}
