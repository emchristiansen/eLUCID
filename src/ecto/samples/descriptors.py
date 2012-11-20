#!/usr/bin/env python
import ecto
from ecto_opencv.highgui import VideoCapture, imshow, FPSDrawer
from ecto_opencv.features2d import DrawKeypoints
from ecto.opts import run_plasm, scheduler_options
from lucid import LUCID
from ecto_opencv.imgproc import cvtColor, Conversion

def parse_args():
    import argparse
    parser = argparse.ArgumentParser(description='Computes the odometry between frames.')
    scheduler_options(parser.add_argument_group('Scheduler'))
    options = parser.parse_args()

    return options

if __name__ == '__main__':
    options = parse_args()

    video = VideoCapture(video_device=0)
    lucid = LUCID()
    draw_kpts = DrawKeypoints()
    rgb2gray = cvtColor (flag=Conversion.RGB2GRAY)
    fps = FPSDrawer()

    plasm = ecto.Plasm()
    plasm.connect(video['image'] >> rgb2gray ['image'],
                rgb2gray['image'] >> lucid['image'],
                lucid['keypoints'] >> draw_kpts['keypoints'],
                video['image'] >> draw_kpts['image'],
                draw_kpts['image'] >> fps[:],
                fps[:] >> imshow('eLUCID display', name='LUCID')['image'],
              )

    run_plasm(options, plasm, locals=vars())
