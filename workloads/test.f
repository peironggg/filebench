define fileset name="testF",entries=10,filesize=15k,prealloc,path="/tmp"
define process name="readerP",instances=3 {
    thread name="readerT",instances=2 {
        flowop openfile name="openOP",filesetname="testF"
        flowop readwholefile name="readOP",filesetname="testF"
        flowop closefile name="closeOP"
    }
}

run 10