##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=orb_extractor
ConfigurationName      :=Release
WorkspacePath          := "${HOME}/webstylix/code"
ProjectPath            := "${HOME}/webstylix/code/lib/orb"
IntermediateDirectory  :=./Release
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Siriwat Kasamwattanarote
Date                   :=04/10/15
CodeLitePath           :="${HOME}/webstylix/configurations/.codelite"
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
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=$(PreprocessorSwitch)NDEBUG 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="orb_extractor.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  `pkg-config opencv --libs`
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)${HOME}/local/include 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)orb $(LibrarySwitch)alphautils $(LibrarySwitch)opencv_core $(LibrarySwitch)opencv_features2d $(LibrarySwitch)opencv_highgui 
ArLibs                 :=  "liborb.a" "libalphautils.a" "opencv_core" "opencv_features2d" "opencv_highgui" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)${HOME}/local/lib $(LibraryPathSwitch)../alphautils/$(ConfigurationName) $(LibraryPathSwitch)./$(ConfigurationName) 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := ar rcu
CXX      := g++
CC       := gcc
CXXFLAGS :=  -O3 -fopenmp -std=c++11 -Wall $(Preprocessors)
CFLAGS   :=  -O2 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/orb_extractor.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./Release || $(MakeDirCommand) ./Release


$(IntermediateDirectory)/.d:
	@test -d ./Release || $(MakeDirCommand) ./Release

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/orb_extractor.cpp$(ObjectSuffix): orb_extractor.cpp $(IntermediateDirectory)/orb_extractor.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "${HOME}/webstylix/code/lib/orb/orb_extractor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/orb_extractor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/orb_extractor.cpp$(DependSuffix): orb_extractor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/orb_extractor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/orb_extractor.cpp$(DependSuffix) -MM "orb_extractor.cpp"

$(IntermediateDirectory)/orb_extractor.cpp$(PreprocessSuffix): orb_extractor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/orb_extractor.cpp$(PreprocessSuffix) "orb_extractor.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Release/


