#!/bin/bash

drush cc all
drush dis -y tbg_base
drush en -y tbg_base
drush tbg-base-im
drush updb
drush tbg-base-if
drush cc all