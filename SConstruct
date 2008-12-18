# -*- python -*-
#
# Setup our environment
#
import glob, os.path, re, os
import lsst.SConsUtils as scons

env = scons.makeEnv("ctrl_events",
                    r"$HeadURL$",
                    [["boost", "boost/version.hpp", "boost_system:C++"],
                     ["boost", "boost/version.hpp", "boost_filesystem:C++"],
                     ["boost", "boost/regex.hpp", "boost_regex:C++"],
                     ["activemqcpp", "activemq/util/Number.h", "activemq-cpp:C++"],
                     ["utils", "lsst/utils/Utils.h", "utils:C++"],
                     ["pex_exceptions", "lsst/pex/exceptions.h","pex_exceptions:C++"],
                     ["daf_base", "lsst/daf/base/Citizen.h", "pex_exceptions daf_base:C++"],
                     ["pex_logging", "lsst/pex/logging/Component.h", "pex_logging:C++"],
                     ["pex_policy", "lsst/pex/policy/Policy.h","pex_policy:C++"],
                     ["python", "Python.h"]
                     ])

#
# Libraries that I need to link things.  This should be handled better
#
#env.libs = dict([
#    ("boost",	env.getlibs("boost_filesystem boost_regex")),
#    ("activemqcpp",	Split("activemq-cpp")),
#    ("python",	Split("python")),
#    ("daf_base",	Split("daf_base"))
#    ])

#
# Build/install things
#
for d in Split("doc lib tests python/lsst/ctrl/events"):
    SConscript(os.path.join(d, "SConscript"))

env['IgnoreFiles'] = r"(~$|\.pyc$|^\.svn$|\.o$)"

Alias("install", env.Install(env['prefix'], "python"))
Alias("install", env.Install(env['prefix'], "include"))
Alias("install", env.Install(env['prefix'], "lib"))
Alias("install", env.Install(env['prefix'] + "/bin", glob.glob("bin/*.py")))
Alias("install", env.InstallEups(env['prefix'] + "/ups", glob.glob("ups/*.table")))

scons.CleanTree(r"*~ core *.so *.os *.o")
#
# Build TAGS files
#
if len(filter(lambda t: t == "TAGS", scons.COMMAND_LINE_TARGETS)) > 0:
    try:
        env.Command("TAGS", scons.filesToTag(), "etags -o $TARGET $SOURCES")
    except AttributeError:                  # not in this version of sconsUtils
        pass

env.Declare()
env.Help("""
LSST Control and Management packages
""")

