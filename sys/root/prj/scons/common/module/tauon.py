#
#The contents of this file are subject to the Mozilla Public License Version 1.1 
#(the "License"); you may not use this file except in compliance with the License.
#You may obtain a copy of the License at http://www.mozilla.org/MPL/
#
#Software distributed under the License is distributed on an "AS IS" basis, 
#WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
#specific language governing rights and limitations under the License.
#
#The Original Code is Lepton.
#
#The Initial Developer of the Original Code is Philippe Le Boulanger.
#Portions created by Philippe Le Boulanger are Copyright (C) 2011 <lepton.phlb@gmail.com>.
#All Rights Reserved.
#
#Contributor(s): Jean-Jacques Pitrolle <lepton.jjp@gmail.com>.
#
#Alternatively, the contents of this file may be used under the terms of the eCos GPL license 
#(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable 
#instead of those above. If you wish to allow use of your version of this file only under the
#terms of the [eCos GPL] License and not to allow others to use your version of this file under 
#the MPL, indicate your decision by deleting  the provisions above and replace 
#them with the notice and other provisions required by the [eCos GPL] License. 
#If you do not delete the provisions above, a recipient may use your version of this file under 
#either the MPL or the [eCos GPL] License."
#
import os
#a source file to compile
class TauonSource:
	def __init__(self, path, opts='-O0', cflags='', cxxflags=''):
		self.path = path
		self.opts = opts
		self.cflags = cflags
		self.cxxflags = cxxflags
		self.env = None
	
	def __str__(self):
		return '[TS: %s %s %s %s]' % (self.path, self.opts, self.cflags, self.cxxflags)

	def set_env(self, env):
		self.env = env

	def compile(self, obj_dir, lib):
		#build complete obj path
		obj_path = ''
		current_env = self.env
		if self.path.find('.cpp') > 0:
			obj_path = obj_dir + os.path.basename(self.path.replace('.cpp','.o'))
		elif self.path.find('.cxx') > 0:
			obj_path = obj_dir + os.path.basename(self.path.replace('.cxx','.o'))
		elif self.path.find('.S') > 0:
			obj_path = obj_dir + os.path.basename(self.path.replace('.S','.o'))
		elif self.path.find('.c') > 0:
			obj_path = obj_dir + os.path.basename(self.path.replace('.c','.o'))
		
		#take lib env if empty or clone it if it's different
		if current_env is None:
			current_env = lib.env
			
		#add special cflags and cxxflags
		if self.cflags != lib.cflags or self.cxxflags != lib.cxxflags:
			current_env = current_env.Clone()
			current_env.Append(CCFLAGS=self.cflags + self.cxxflags)
			
		#print 'make object from ' + obj_path + ' to ' + self.path + ' opts : ' + self.opts
			
		#return object node
		return current_env.Object(obj_path, self.path)
				
#a library contains a list of source files
#inherit from TauonSource
class TauonLibrary(TauonSource):
	def __init__(self, path, opts='-O0', cflags='', cxxflags=''):
		TauonSource.__init__(self, path, opts, cflags, cxxflags)
		self.src_lst = []
		
	def add_sources(self, tauon_srcs):
		self.src_lst.extend(tauon_srcs)
	
	def compile(self, obj_dir):
		#compile all sources
		obj_lst = []
		lib_name = os.path.basename(self.path)
		lib_dir = os.path.dirname(self.path)
		#print 'libname:' + lib_name + ' libdir:'+ lib_dir + ' opts:' + self.opts
		
		for i in range(len(self.src_lst)):
			obj_lst.extend([self.src_lst[i].compile(obj_dir, self)])
		
		#make library and intall it
		library = self.env.StaticLibrary(lib_name, obj_lst)
		self.env.Install(lib_dir, library)
		self.env.Alias(lib_name, lib_dir)
		
		return self.path

class TauonApplication(TauonLibrary):
	def __init__(self, path='', opts='-O0', cflags='', cxxflags=''):
		TauonLibrary.__init__(self, path, opts, cflags, cxxflags)
		self.lib_lst = []
		self.obj_node_lst = []
		
	def set_name(self, path):
		self.path = path

	def add_lib(self, lib_name):
		self.lib_lst.extend(lib_name)

	def compile(self, obj_dir):
		#compile all sources
		for i in range(len(self.src_lst)):
			self.obj_node_lst.extend([self.src_lst[i].compile(obj_dir, self)])
	
	def build(self):
		bin_dir = os.path.dirname(self.path)
		bin_name = os.path.basename(self.path)
		libs_dir_lst = []
		libs_name_lst = []
		#
		for i in range(len(self.lib_lst)):
			libs_dir_lst.extend([os.path.dirname(self.lib_lst[i])])
			libs_name_lst.extend([os.path.basename(self.lib_lst[i])])

		#make binary
		binary = self.env.Program(bin_name, self.obj_node_lst, PROGSUFFIX='.elf',
		LIBS=libs_name_lst, LIBPATH=libs_dir_lst)
		
		#and install it
		self.env.Install(bin_dir, binary)
		self.env.Alias('bin', bin_dir)

#		
class TauonEnvironment:
	def __init__(self, default_env):
		self.envs_map = {'DEFAULT':default_env}
	
	def configure_envs(self, opts_lst):
		for i in range(len(opts_lst)):
			self.envs_map[opts_lst[i]] = self.envs_map['DEFAULT'].Clone()
			self.envs_map[opts_lst[i]].Replace(OPTS = opts_lst[i])
			self.envs_map[opts_lst[i]].Append(CCFLAGS = opts_lst[i])
			
	def add_env(self, key, value):
		self.envs_map[key] = value

#a test
if __name__ == '__main__':
	print 'A simple tauon module to compile application'
