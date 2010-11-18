#!/bin/bash
DIR="$( cd "$( dirname "$0" )" && pwd )"
rsync -av --exclude ".svn/" --exclude ".git/" --exclude "CVS/" $DIR/../players/osmplayer/player $DIR/../../../../../../drupal7/sites/all/modules/mediafront/players/osmplayer
