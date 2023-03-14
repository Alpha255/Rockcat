import sys
import os
import argparse

def ExecCommand(Command):
	try:
		Ret = os.system(Command)
		if Ret != 0:
			raise Exception('Commandline done with error: {0}'.format(Command))
	except OSError as Err:
		print('Failed to execute commandline: {0}, Error info: {1}'.format(Command, Err.__str__()))

def PreMakeAssimp():
	try:
		AssimpBuildPath = '.\\Submodules\\assimp\\build'
		if not os.path.exists(AssimpBuildPath):
			os.mkdir(AssimpBuildPath)
		print('Build: Premake assimp...')
		ExecCommand('cmake -S .\\Submodules\\assimp -B .\\Submodules\\assimp\\build -G "Visual Studio 17 2022" -A x64')
	except OSError as Err:
		print('Failed to make directory: {0}'.format(Err.__str__()))

def PreMake():
	print('Build: Premake...')
	ExecCommand(os.path.join(os.getcwd(), 'premake-5.0.0-beta2-windows\\premake5 ') + '--scripts=' + os.path.join(os.getcwd() + 'premake.lua vs2022'))

if __name__ == "__main__":
	PreMakeAssimp()
	PreMake()