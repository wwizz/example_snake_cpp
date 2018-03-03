#!groovy

class JenkinsJobInfo implements Serializable {
    def env

    JenkinsJobInfo(jenEnv) {
        this.env = jenEnv
    }

    String get_jobname() {
        return "${this.env.JOB_NAME}".tokenize( '/' )[0]
    }

    String get_branch_name() {
        try {
            return this.env.BRANCH_NAME.replace("/", "_")
        } catch (error) {
            return this.env.BRANCH.replace("/", "_")
        }
    }

    String get_build_nr() {
        return this.env.BUILD_NUMBER
    }

    String get_build_base_dir() {
        //Adjust here for the location of the temp build dir of the job
        //now we choose to base a short path because of path limitations on windows
        //Currenly Jenkins does not support changing the base dir for (multibranch) pipeline jobs through its job/system configuration
        //Perhaps make it plaform dependent and/or base it on environemnt variables
        return "/b"
    }

    boolean is_fast_build() {
        //Adjust here to determine when script should consider this to be a 'fast' build
        //now we choose to base the decision on the name of the job
        return this.get_jobname().endsWith("fast")
    }
}

class BuildTask implements Serializable {                                  
    String platform
    String compiler
    String option
    JenkinsJobInfo job_info

    String get_stage_name() {
        return "${this.platform}_${this.compiler}_${this.option}"
    }

    String get_short_stage_name() {
        return "${this.compiler}_${this.option}"
    }

    String get_source_directory_name() {
        return "${job_info.get_build_base_dir()}/${job_info.get_jobname()}_${this.get_short_stage_name()}_${job_info.get_build_nr()}"
    }

    String get_build_directory_name() {
        return this.get_source_directory_name() + "/build"
    }

    String get_source_directory() {
        return new File(this.get_source_directory_name()).getAbsolutePath()
    }

    String get_build_directory() {
        return new File(this.get_build_directory_name()).getAbsolutePath()
    }

    String[] get_cmake_build_commands() {
        //maybe convert the swithes here to a map that is injected to this class
        def opts = ''
        def build_opts = ''
        def targets = [];
        targets += "huestream"
        targets += "huestream_example_console"
        targets += "huestream_tests"

        switch (this.option) {
            case "min":
                opts = '-DBUILD_WRAPPERS=Off -DBUILD_SWIG=Off -DBUILD_WITH_RTTI=Off -DBUILD_WITH_EXCEPTIONS=Off'
                break
            case "wrap":
                opts = '-DBUILD_WRAPPERS=On -DBUILD_SWIG=On -DBUILD_WITH_RTTI=On -DBUILD_WITH_EXCEPTIONS=On'
                targets += "huestream_csharp_native"
                targets += "huestream_csharp_managed"
                targets += "huestream_java_native"
                break
            case "cov":
                opts = '-DBUILD_WRAPPERS=On -DBUILD_SWIG=On -DBUILD_WITH_RTTI=On -DBUILD_WITH_EXCEPTIONS=On -DBUILD_TEST_COV=On'
                targets += "huestream_tests_coverage"
                break
            default:
                break
        }
        
        def gen = ''
        switch (this.compiler) {
            case "VS2015_32":
                gen = '-G "Visual Studio 14 2015"'
                targets += "huestream_example_gui_win"
                targets += "huestream_performance_test"
                if (this.option == "wrap") {
                    targets += "huestream_csharp_example_wpf"
                }
                break
            case "VS2015_64":
                gen = '-G "Visual Studio 14 2015 Win64"'
                targets += "huestream_example_gui_win"
                targets += "huestream_performance_test"
                if (this.option == "wrap") {
                    targets += "huestream_csharp_example_wpf"
                }
                break
            case "MINGW":
                gen = '-G "MinGW Makefiles"'
                build_opts = ' -- -j40'
                break
            case "gcc-debug":
                gen = '-DCMAKE_BUILD_TYPE=Debug'
                break
            case "gcc-release":
                gen = '-DCMAKE_BUILD_TYPE=Release'
                break
            case "clang-debug":
                gen = '-DCMAKE_BUILD_TYPE=Debug'
                break
            case "clang-release":
                gen = '-DCMAKE_BUILD_TYPE=Release'
                break
            default:
                break
        }
        def commands = []
        commands += "cmake ${gen} ${opts} .."
        targets.each{ t -> commands += "cmake --build . --target ${t} ${build_opts}"}
        return commands;
    }

    String[] get_unittest_commands() {
        def commands = []
        commands += './bin/huestream_tests'
        return commands;
    }
}

// *********************************************************************************************************
//  Helper (functions)
// *********************************************************************************************************
void execute_command(platform, command) {
    if (platform == 'windows') {
        command = command.replace("/", "\\")
        bat(command) 
    } else {
        sh(command)
    }
}

def get_platforms_not_online(tasks) {
    // get plaform nodes needed
    platforms_not_online = []
    tasks.each { t ->
        platforms_not_online += t.platform
    }

    //Get all labels of available nodes
    def labels = []
    Jenkins.instance.labelString.split().each { 
        label -> labels += label 
    }
    Jenkins.instance.slaves.each { 
        slave -> slave.labelString.split().each { 
            label -> 
            if (!slave.getComputer().isOffline()) {
                labels += label 
            }
        }
    }

    //Remove all platforms that are online form the list
    labels.each({ label -> platforms_not_online.removeAll({ platform -> platform == label})})

    
    if (platforms_not_online.size() != 0) {
        platforms_not_online.each { platform ->
            tasks.removeAll {t-> platform == t.platform}
        }
    }

    return platforms_not_online
}

// *********************************************************************************************************
//  Stage (functions)
// *********************************************************************************************************
def build_stage(BuildTask task) {
    node(task.platform) {

        stage("build_${task.get_stage_name()}"){
            def source_dir = task.get_source_directory()
            def build_dir = task.get_build_directory()
            def stage_name = task.get_stage_name()
            try {
                println "--- Preparing build ..."
                dir(source_dir) {
                    unstash 'source'
                }

                println "--- Building ... "
                dir(build_dir) {
                    def commands = task.get_cmake_build_commands();
                    commands.each{ t -> execute_command(task.platform, t)}
                    stash name: "binaries_${stage_name}", includes: 'bin/**/*'
                }

                println "--- Archiving ... "
                dir(build_dir + "/artifacts_${stage_name}") {
                    unstash name: "binaries_${stage_name}"
                }

                dir(build_dir) {
                    archiveArtifacts artifacts: "artifacts_${stage_name}/**/*"
                }


            } catch(ex) {
                println "Error occured during build ... "
                throw ex
            } finally {
                println "--- Cleaning ... "
                dir(build_dir) {
                    deleteDir()
                }
                dir(source_dir) {
                    deleteDir()
                }
            } 
        }
    }
}


def test_stage(BuildTask task) {
    node(task.platform) {
        stage('test_' + task.get_stage_name()){
            dir('test') {
                println "--- Unittesting ... "
                unstash name: "binaries_${task.get_stage_name()}"
                def commands = task.get_unittest_commands();
                commands.each{ t -> execute_command(task.platform, t)}
            }
            if (task.option == 'cov') {
                publishHTML([allowMissing: false, alwaysLinkToLastBuild: false, keepAll: true, reportDir: 'test/bin/coverage', reportFiles: 'huestream_tests_coverage.html', reportName: "Coverage_${task.get_stage_name()}", reportTitles: ''])
            }
        }
    }
}

// *********************************************************************************************************
// Main
// *********************************************************************************************************
def build_jobs_for_parallel_execution = [:]

stage('checkout') {
    node('linux-prince') {

        println " --- Gathering build task"
        
        // Note:: 
        // Building is done in directories created by this script rather than Jenkins created directories 
        // because of path length issues on windows :( Currenly Jenkins mutlibranch pipeline does not 
        // support changing the naming strategy and generates very long paths.
        //
        // Workaround is to add the following variable: 'jenkins.branch.WorkspaceLocatorImpl.PATH_MAX=0' to 
        // JAVA_ARGS variable in the  '/etc/default/jenkins' file by for example: 
        // JAVA_ARGS="-Djenkins.branch.WorkspaceLocatorImpl.PATH_MAX=0" which generates significant smaller
        // path names. However still the checkout paths are too long sometimes 
        // 
        // Solid workaround is to create/delete *own build dirs* with a short as possible names 
        // See JenkinsJobInfo class that defines the working dirs based on Jenkins environment vars
        //
        // Any other attemps to change checkout dir was unsuccessful at time of script creation. Even with
        // Windows10 now supporting long paths. It seems that also JAVA needs to support the new longer paths
        // on windows
        
        JenkinsJobInfo job_info = new JenkinsJobInfo(env);


        // Note::
        // Define the tasks to execute in the code below
        // See JenkinsJobInfo.is_fast_build() when buid is considered a fast_build
        // In the furture, perhaps extend this script for more flavors, like 'all', 'fast, 'windows-only', ...
        // 
        // See the BuildTask class for mapping of compiler and option to cmake commands and flags...
        // In the furture, perhaps extend this script to this mapping is done based on a table rather than code 
        // switch statemement ...

        def tasks = []

        if (job_info.is_fast_build()) {
            tasks = [
                new BuildTask(job_info: job_info, platform: "linux",   compiler: "gcc-debug",   option: "cov"),
                new BuildTask(job_info: job_info, platform: "mac",     compiler: "clang-debug", option: "cov"),
            ]
        } else {
            tasks = [
                new BuildTask(job_info: job_info, platform: "windows", compiler: "VS2015_32",   option: "min"),
                new BuildTask(job_info: job_info, platform: "windows", compiler: "VS2015_64",   option: "wrap"),
                new BuildTask(job_info: job_info, platform: "windows", compiler: "MINGW",       option: "min"),
                new BuildTask(job_info: job_info, platform: "linux",   compiler: "gcc-release", option: "min"),
                new BuildTask(job_info: job_info, platform: "linux",   compiler: "gcc-debug",   option: "cov"),
                new BuildTask(job_info: job_info, platform: "mac",     compiler: "clang-debug", option: "wrap"),
            ]
        }

        // report build tasks
        if (job_info.is_fast_build()) {
            println "NOTE: Job is a fast build"
        } else {
            println "NOTE: Job is a normal build"
        }

        tasks.each { t ->
            println "NOTE: BuildTask ${t.platform}_${t.compiler}_${t.option} selected"
        }

        println " --- Checking nodes"
        def platforms_not_online = get_platforms_not_online(tasks)

        // report nodes
        if (platforms_not_online.size() != 0) {
            println "WARNING: Job cannot execute on all required nodes, build UNSTABLE!"
            currentBuild.result = 'UNSTABLE'

            platforms_not_online.each { platform ->
                println "WARNING: node ${platform} offline"
            }
        } else {
            println "NOTE: All nodes found"
        }

        //convert tasks to jobs (closures)
        tasks.each{ task ->
            build_jobs_for_parallel_execution[task.get_stage_name()] = { 
                build_stage(task) 
                test_stage(task)
            }
        }

        println " --- Checking out source code"
        checkout scm

        if (fileExists('tools/export-edk/export-edk.sh')) {
            // running on sdk -- export the edk first
            sh './tools/export-edk/export-edk.sh exported_edk false'

            dir('exported_edk') {
                stash name: 'source'
            }
        } else if (fileExists('libhuestream/CMakeLists.txt')) {
            // running on standalone edk
            stash name: 'source'
        } else {
            fail("Could not find edk or sdk")
        }
    }
}

stage('build') {
    parallel(build_jobs_for_parallel_execution)
}

stage('post') {
    node('master') {
        warnings canComputeNew: false, canResolveRelativePaths: false, categoriesPattern: '', consoleParsers: [[parserName: 'CppLint'], [parserName: 'Clang (LLVM based)'], [parserName: 'GNU C Compiler 4 (gcc)']], defaultEncoding: '', excludePattern: '', healthy: '', includePattern: '', messagesPattern: '', unHealthy: ''
    }
}
