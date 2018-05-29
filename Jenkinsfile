properties([disableConcurrentBuilds(), [$class: 'BuildDiscarderProperty', strategy: [$class: 'LogRotator', artifactDaysToKeepStr: '', artifactNumToKeepStr: '', daysToKeepStr: '', numToKeepStr: '15']]]);

def buildCommands = [
    '"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON -DOPENKIT_32_BIT=OFF ../',
    '"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=OFF -DOPENKIT_32_BIT=OFF ../',
    '"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DOPENKIT_32_BIT=OFF ../',
    '"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DOPENKIT_32_BIT=OFF ../'
    ]
def builds = [:]

def compilers = [:]
compilers["gcc"] = "CXX=/usr/bin/c++ CC=/usr/bin/cc"
compilers["clang"] = "CXX=/usr/bin/clang++ CC=/usr/bin/clang"

buildCommands.each{
    compilers.each{compiler -> 
        builds[compiler.key + " " + it] = {
            node("openkit-ubuntu-native") {
                checkout scm
            
                dir('build') {
                    deleteDir()
                }
            
                dir('build') {
                    sh("${compiler.value} cmake -G${it}")
                    sh("${compiler.value} make clean all -j8")
                    try {
                        sh("./bin/OpenKitTest --gtest_output=xml:testreport.xml")
                    } finally {
                        junit("testreport.xml")
                    }
                }
            }
        }
    }
}
stage("build-and-test") {
    parallel(builds)
}