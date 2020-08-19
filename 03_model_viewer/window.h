//
// Created by fede on 19/08/20.
//

#pragma once

#include "model_viewer.h"

static int WindowWidth = 1366;
static int WindowHeight = 768;
static GLfloat aspect_ratio = 16.0f / 9.0f;

void init_window(int argc, char **argv);
void resize(int w, int h);
void printToScreen();
