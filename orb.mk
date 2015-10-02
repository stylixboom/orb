##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=orb
ConfigurationName      :=Release
WorkspacePath          := "/home/stylix/Dropbox/MyDocument/SokendaiCourse/Researches/Workspace/code"
ProjectPath            := "/home/stylix/Dropbox/MyDocument/SokendaiCourse/Researches/Workspace/code/lib/orb"
IntermediateDirectory  :=./Release
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Siriwat Kasamwattanarote
Date                   :=02/10/15
CodeLitePath           :="/home/stylix/Dropbox/MyDocument/SokendaiCourse/Researches/Workspace/configurations/.codelite"
LinkerName             :=g++
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/lib$(ProjectName).a
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="orb.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)/home/stylix/local/include 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)/home/stylix/local/lib 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := ar rcu
CXX      := g++
CC       := gcc
CXXFLAGS :=  -O3 -std=c++11 $(Preprocessors)
CFLAGS   :=   $(Preprocessors)
ASFLAGS  := 
AS       := as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/orb.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(IntermediateDirectory) $(OutputFile)

$(OutputFile): $(Objects)
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(AR) $(ArchiveOutputSwitch)$(OutputFile) @$(ObjectsFileList) $(ArLibs)
	@$(MakeDirCommand) "/home/stylix/Dropbox/MyDocument/SokendaiCourse/Researches/Workspace/code/.build-release"
	@echo rebuilt > "/home/stylix/Dropbox/MyDocument/SokendaiCourse/Researches/Workspace/code/.build-release/orb"

MakeIntermediateDirs:
	@test -d ./Release || $(MakeDirCommand) ./Release


./Release:
	@test -d ./Release || $(MakeDirCommand) ./Release

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/orb.cpp$(ObjectSuffix): orb.cpp $(IntermediateDirectory)/orb.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/stylix/Dropbox/MyDocument/SokendaiCourse/Researches/Workspace/code/lib/orb/orb.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/orb.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/orb.cpp$(DependSuffix): orb.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/orb.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/orb.cpp$(DependSuffix) -MM "orb.cpp"

$(IntermediateDirectory)/orb.cpp$(PreprocessSuffix): orb.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/orb.cpp$(PreprocessSuffix) "orb.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Release/


