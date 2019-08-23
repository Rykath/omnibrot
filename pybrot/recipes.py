"""
recipes.py
by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
"""

# --- Imports --- #

import subprocess
from collections.abc import Callable

# --- Utility --- #


def run(args):
    return subprocess.run(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, encoding='UTF-8')


# --- Deployed Recipes --- #


class DeployedRecipe(Callable):
    def __init__(self, **kwargs):
        for key, value in kwargs.items():
            self.__setattr__(key, value)


class LocallyCompiled(DeployedRecipe):
    def __init__(self, recipe, path):
        self.recipe = recipe
        self.path = path

    def __call__(self, instructions, outputfile, printdirect=True):
        proc = run([self.path]+self.recipe.arguments(instructions, outputfile))
        if printdirect:
            print(proc.stdout)
            print(proc.stderr)


# --- Recipes --- #


class Recipe:
    repo_dir = '.'
    repo_bin = ''

    def __init__(self, **kwargs):
        for key, value in kwargs.items():
            self.__setattr__(key, value)

    def getdeployed(self, cls=LocallyCompiled):
        if isinstance(cls, LocallyCompiled):
            return cls(self, self.repo_dir+'/'+self.repo_bin)


class Mandelbrot(Recipe):
    def arguments(self, instructions, outputfile):
        r = []
        for key in ['samples', 'center', 'range']:
            for axis in ['real', 'imag']:
                r.append(str(instructions[key][axis]))
        r.append(str(instructions['maximum-iteration']))
        r.append(outputfile)
        return r


class MandelFPHPN(Mandelbrot):
    repo_dir = 'recipes'
    repo_src = 'mandel_fphpn.cpp'
    repo_bin = 'mandel-FPHPN'


class MandelRel2Cen(Mandelbrot):
    repo_dir = 'recipes'
    repo_src = 'mandel_rel2cen.cpp'
    repo_bin = 'mandel-Rel2Cen'
