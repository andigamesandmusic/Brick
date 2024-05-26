--------------------------------------------------------------------------------
--                                 Options
--------------------------------------------------------------------------------
public = false
SettingsFilename = "Settings.lua"
ICNSFilename = "Icon.icns"
ICONRCFilename = "Icon.rc"

ExtraGCCWarnings = {"-Wextra", "-pedantic",
                    "-Wno-long-long", --For now, hide warnings about long long.
                    "-fno-strict-aliasing"} --For now, disable strict aliasing.

ExtraVSWarnings = {"/W4"}
NoVSWarnings = {"/W0"}
NoGCCWarnings = {"-w"}
HiddenSymbolVisibility = {"-fvisibility=hidden"}
BuildFor10_4MacOSXSystems = {"-mmacosx-version-min=10.4"}
BuildFor10_5MacOSXSystems = {"-mmacosx-version-min=10.5"}
ExtendedDefines = {"USING_EXPERIMENTAL", "USING_FLOATX"}
ExtendedLibPath = {"/usr/local/lib"}
ExtendedIncludePath = {"/usr/local/include"}
Xcode3Platforms = {"native", "universal", "universal32"}
WindowsPlatforms = {"native", "x64"}
X11LibPath = "/usr/X11R6/lib/"
DebugFlags = {"Symbols", "NoPCH", "NoManifest"}
ReleaseFlags = {}
if os.is("linux") then
  ReleaseFlags = {"NoPCH", "NoManifest"} --On Linux, optimization is breaking.
else
  ReleaseFlags = {"OptimizeSpeed", "NoPCH", "NoManifest"}
end

MacOSXFrameworks = {"-framework Carbon", "-framework Cocoa",
  "-framework CoreServices", "-framework CoreAudio", "-framework CoreMidi",
  "-framework ApplicationServices", "-framework AGL", "-framework IOKit",
  "-framework WebKit"}
  
JUCEDefines = {"JUCE_ASIO=0", "JUCE_DIRECTSOUND=0", "JUCE_JACK=0",
  "JUCE_QUICKTIME=0", "JUCE_OPENGL=0", "JUCE_USE_FLAC=0",
  "JUCE_USE_OGGVORBIS=0", "JUCE_USE_CDBURNER=0", "JUCE_USE_CDREADER=0",
  "JUCE_WEBBROWSER=0", "JUCE_CHECK_MEMORY_LEAKS=0"}

JUCELinuxLinks = {"freetype", "pthread", "rt", "X11", "GL", "GLU", "Xinerama",
                  "asound"}
                  
ExtendedLinks = {"fftw3", "sndfile", "fftw3_threads", "m", "pthread"}

--------------------------------------------------------------------------------
--                                  Paths
--------------------------------------------------------------------------------

PathToRoot = ""
PathToSrc = "src/"
PathToPrim = "src/prim/"
PathToJUCE = "src/juce/"
PathToLib = "lib/"
PathToBin = "bin/"
PathToObj = "obj/"
PathToRes = "res/"
SafeToDeleteBin = true

--------------------------------------------------------------------------------
--                            Helper Functions
--------------------------------------------------------------------------------

--[[This will generate the same UUIDs in the same order. This prevents SVN from
needing to recommit files for Microsoft Visual Studio since the UUID will always
be predictable.]]
function GenerateUUID()
  --Here is a random stem, to which the rest of the of UUID will be appended.
  g = "7B5AFD3D-53E6-4A1C-9864-"
  for i = 1, 12 do g = g .. math.random(0, 9) end
  return g
end

--Gets the correct relative path from the current location.
function FromRoot(PathFromRoot)
  return PathToRoot .. PathFromRoot
end

--Determines if the file contains a certain search string.
function FileContains(filename, searchstring)
  filehandle = io.input(filename)
  if string.find(filehandle:read(-1), searchstring) then
    io.close(filehandle)
    return true
  else
    io.close(filehandle)
    return false
  end
end

--------------------------------------------------------------------------------
--                        Belle, Bonne, Sage Solution
--------------------------------------------------------------------------------
solution "bellebonnesage"
  --Create a release and debug configuration for each project.
  configurations {"Release", "Debug"}
  
  --For Xcode 3 automatically include the Universal targets.
  if(_ACTION == "xcode3") then platforms(Xcode3Platforms) end
  
  --For Windows automatically include the 64-bit targets.
  if(_ACTION == "vs2008" or _ACTION == "vs2005") then
    platforms(WindowsPlatforms)
  end
  
--------------------------------------------------------------------------------
--                        Belle, Bonne, Sage Library
--------------------------------------------------------------------------------

project "bellebonnesage"
  uuid(GenerateUUID())
  language "C++"
  flags {"StaticRuntime", "ExtraWarnings"}
  files {FromRoot(PathToPrim) .. "**.cpp", FromRoot(PathToPrim) .. "**.h"}
  kind "StaticLib"
  objdir(FromRoot(PathToObj) .. "bellebonnesage")
  targetdir(FromRoot(PathToLib))
  defines(ExtendedDefines)
  
  --[[Fix hidden symbol visibility warning, add extra warnings, and build for as
  many versions of Mac OS X as possible.]]
  configuration "xcode3"
    buildoptions({HiddenSymbolVisibility, ExtraGCCWarnings,
      BuildFor10_4MacOSXSystems})
  
  --Turn on extra GCC warnings and compile against the ISO specification.
  configuration "gmake"
    buildoptions(ExtraGCCWarnings)

  --Add extra defines for Lua and signal operating system environment for prim.
  configuration "linux"
    defines("PRIM_LINUX")
    
  --Set defines and flags.
  configuration "windows" defines("PRIM_WINDOWS")   
  configuration "macosx" defines("PRIM_MACOSX")
  configuration "Debug" flags(DebugFlags) 
  configuration "Release" flags(ReleaseFlags)
  
--------------------------------------------------------------------------------
--                               JUCE Library
--------------------------------------------------------------------------------

project "juce"
  uuid(GenerateUUID())
  language "C++"
  flags {"StaticRuntime"}
  files {FromRoot(PathToJUCE) .. "*.cpp", FromRoot(PathToJUCE) .. "*.h"}
  kind "StaticLib"
  objdir(FromRoot(PathToObj) .. "juce")
  targetdir(FromRoot(PathToLib))
  defines(JUCEDefines)
  
  --On Mac, use the Objective-C++ file instead of the CPP file.
  configuration "macosx"
    files {FromRoot(PathToJUCE) .. "*.mm"}
    excludes {FromRoot(PathToJUCE) .. "*.cpp"}
  
  --[[Correct the symbol visibility problem and turn off warnings in JUCE since
  it is beyond the scope of this project to correct them.]]
  configuration "xcode3"
    buildoptions({HiddenSymbolVisibility, NoGCCWarnings})
  
  --Same as above.
  configuration "gmake"
    buildoptions(NoGCCWarnings)
    
  --JUCE can not build with a target system earlier than Mac OS X 10.4.
  configuration {"xcode3", "universal32"}
    buildoptions(BuildFor10_4MacOSXSystems)

  --[[In order to build including both 32 and 64-bit architectures we need to
  limit the earliest target Mac OS to 10.5 and later since some things are not
  supported on 64-bit architectures until 10.5 such as zerocost exceptions.]]
  configuration {"xcode3", "universal"}
    buildoptions(BuildFor10_5MacOSXSystems)
  
  --On Linux we need to specify the include directory for FreeType
  configuration {"linux"}
    includedirs("/usr/include/freetype2")

  configuration "Debug" flags(DebugFlags) 
  configuration "Release" flags(ReleaseFlags)

--------------------------------------------------------------------------------
--                                   Brick
--------------------------------------------------------------------------------

ProjectName = "brick"
PathToProject = PathToSrc

project(ProjectName)
  uuid(GenerateUUID())
  language "C++"
  kind "ConsoleApp"
  flags {"StaticRuntime", "WinMain"}
  links {"bellebonnesage", "juce"}
  links(ExtendedLinks)
  defines(ExtendedDefines)
  libdirs(ExtendedLibPath)
  includedirs(ExtendedIncludePath)
  files {FromRoot(PathToProject) .. "*.cpp",
         FromRoot(PathToProject) .. "*.h"}
  includedirs {FromRoot(PathToPrim)}
  libdirs {FromRoot(PathToLib)}
  objdir(FromRoot(PathToObj) .. ProjectName)
  targetdir(FromRoot(PathToBin))
  defines(JUCEDefines)
  
  --Link to the Mac OS X frameworks required by JUCE.
  configuration "macosx"
    linkoptions(MacOSXFrameworks)

  --See JUCE library project above for explanation.
  configuration {"xcode3", "universal32"}
    buildoptions(BuildFor10_4MacOSXSystems)
    linkoptions(BuildFor10_4MacOSXSystems)

  --See JUCE library project above for explanation.
  configuration {"xcode3", "universal"}
    buildoptions(BuildFor10_5MacOSXSystems)
    linkoptions(BuildFor10_5MacOSXSystems)

  --Add some extra Linux flags and links for Lua.
  configuration "linux"
    defines(LuaLinuxDefines)
    libdirs(X11LibPath)
    links(LuaLinuxLinks)
    links(JUCELinuxLinks)
    
  configuration "Debug" flags(DebugFlags) 
  configuration "Release" flags(ReleaseFlags)

print("")


--------------------------------------------------------------------------------
--                                Brick GUI
--------------------------------------------------------------------------------

ProjectName = "BrickGUI"
PathToProject = PathToSrc
PathToGUI = PathToSrc .. "gui/"
MacOSXTargetNameEscaped = "Brick"

project(ProjectName)
  uuid(GenerateUUID())
  language "C++"
  kind "WindowedApp"
  flags {"StaticRuntime", "WinMain"}
  links {"bellebonnesage", "juce"}
  links(ExtendedLinks)
  defines(ExtendedDefines)
  libdirs(ExtendedLibPath)
  includedirs(ExtendedIncludePath)
  files {FromRoot(PathToProject) .. "*.cpp",
         FromRoot(PathToProject) .. "*.h",
         FromRoot(PathToGUI) .. "*.cpp",
         FromRoot(PathToGUI) .. "*.h"}
  excludes {FromRoot(PathToProject) .. "Main.*"}
  includedirs {FromRoot(PathToPrim), FromRoot(PathToProject)}
  libdirs {FromRoot(PathToLib)}
  objdir(FromRoot(PathToObj) .. ProjectName)
  targetdir(FromRoot(PathToBin))
  targetname(MacOSXTargetNameEscaped)
  defines(JUCEDefines)
  
  --Link to the Mac OS X frameworks required by JUCE.
  configuration "macosx"
    linkoptions(MacOSXFrameworks)

  --See JUCE library project above for explanation.
  configuration {"xcode3", "universal32"}
    buildoptions(BuildFor10_4MacOSXSystems)
    linkoptions(BuildFor10_4MacOSXSystems)

  --See JUCE library project above for explanation.
  configuration {"xcode3", "universal"}
    buildoptions(BuildFor10_5MacOSXSystems)
    linkoptions(BuildFor10_5MacOSXSystems)
    
  --Add icon and Info.plist on Mac OS X.
  configuration {"macosx", "gmake"}
    if os.isfile(FromRoot(PathToGUI) .. "Info.plist") then
      postbuildcommands("cp " .. FromRoot(PathToGUI) .. 
      "Info.plist " .. FromRoot(PathToBin) .. MacOSXTargetNameEscaped .. 
      ".app/Contents")
    end
    
    if os.isfile(FromRoot(PathToGUI) .. ICNSFilename) then
      postbuildcommands("mkdir -p " .. FromRoot(PathToBin) .. 
        MacOSXTargetNameEscaped .. ".app/Contents/Resources")
      postbuildcommands("cp " .. FromRoot(PathToGUI) .. 
        ICNSFilename .. " " .. FromRoot(PathToBin) ..
        MacOSXTargetNameEscaped .. ".app/Contents/Resources")
    end

  --Add some extra Linux flags and links for Lua.
  configuration "linux"
    defines(LuaLinuxDefines)
    libdirs(X11LibPath)
    links(LuaLinuxLinks)
    links(JUCELinuxLinks)
    
  configuration "Debug" flags(DebugFlags) 
  configuration "Release" flags(ReleaseFlags)

print("")

--------------------------------------------------------------------------------
--                                 Cleanup
--------------------------------------------------------------------------------

if _ACTION == "clean" then
  print("Removing target and object directories.")
  os.rmdir(FromRoot(PathToLib))
  if SafeToDeleteBin then
    os.rmdir(FromRoot(PathToBin))
  end
  os.rmdir(FromRoot(PathToObj))
end
