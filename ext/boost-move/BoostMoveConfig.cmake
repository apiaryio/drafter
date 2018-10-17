include(CMakeFindDependencyMacro)
find_dependency(BoostCore 1.66)
find_dependency(BoostStaticAssert 1.66)
include("${CMAKE_CURRENT_LIST_DIR}/BoostMoveTargets.cmake")
