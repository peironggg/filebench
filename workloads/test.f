define fileset name="testF",entries=10,filesize=5k,prealloc,path="/tmp"
define process name="readerP",instances=1 {
    thread name="readerT",instances=1 {
        flowop openfile name="openOP",filesetname="testF"
        flowop readwholefile name="readOP",filesetname="testF"
        flowop closefile name="closeOP"
    }
}

run 10