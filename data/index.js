
// const listOfFiles = ["breakingbad.jpg", "coherence.jpg", "dark.jpg", "tgb.jpg", "thresh.jpg"];
// const parent = document.createElement('div')
// parent.setAttribute("id", "download__list")
// listOfFiles.forEach((element, index) => {
//     let el = document.createElement("a");
//     el.setAttribute('download', `${element.slice(0, -4)}`);
//     el.setAttribute('href', `./files/${element}`)
//     let newContent = document.createTextNode(element.slice(0, -4));
//     el.appendChild(newContent);
//     document.body.append(parent)
//     parent.appendChild(el)
// })

let createFooter = document.createElement("footer");
const filesToUpload = document.getElementById("files_name");

let arFiles = [];
Dropzone.options.myDropzone = {

    autoProcessQueue: false,
    uploadMultiple: true,
    parallelUploads: 100,
    maxFiles: 100,

    init: function() {
        this.on("addedfile", file => {
        if(file)
            filesToUpload.insertAdjacentHTML("beforeend", "<div class='loading'><p>"+file.name+"</p> <img src='/spinner.gif' width='30' height='30'/></div>");
            uploadFile(file);
        });
    }
};

document.body.appendChild(createFooter);
let githubs = [{
        name: "Timothée",
        username: "timoogo"
    },
    {
        name: "Ulrich",
        username: "ngakui"
    },
    {
        name: "Christian",
        username: "christianyoha"
    }
]
githubs.forEach(element => {
    let el = document.createElement("a");
    el.setAttribute('href', `https://github.com/${element.username}`)
    let newContent = document.createTextNode(`Github : ${element.name}`);
    el.appendChild(newContent);
    createFooter.appendChild(el);
})

createFooter.appendChild(githubs)



// SCRIPTS FOR ESP 32

function uploadFile(file) {
    let formdata = new FormData();
    formdata.append("file", file);

    let ajax = new XMLHttpRequest();
    ajax.upload.addEventListener("progress", progressHandler, false);
    ajax.addEventListener("load", completeHandler, false); // doesnt appear to ever get called even upon success
    ajax.addEventListener("error", errorHandler, false);
    ajax.addEventListener("abort", abortHandler, false);
    ajax.open("POST", "/");
    ajax.send(formdata);
}

function progressHandler(event, file) {
    
}

function completeHandler(event) {
    document.getElementById("status").innerHTML = "<p>Successfully</p>";
    filesToUpload.innerHTML = "";
    // getListFiles();
    let xmlhttp =new XMLHttpRequest();
    xmlhttp.open("GET", "/listfiles", false);
    xmlhttp.send();
    document.getElementById("download__list").innerHTML = xmlhttp.responseText;
}

function errorHandler(event) {
    document.getElementById("status").innerHTML = "Upload Failed";
}

function abortHandler(event) {
    document.getElementById("status").innerHTML = "Upload Aborted";
}

function getListFiles(){
    let xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function()
    {
        if(this.readyState == 4 && this.status == 200)
        {
            document.getElementById("download__list").innerHTML = xhttp.responseText;
            console.log(xhttp);
        }
    };
    xhttp.open("GET", "/listfiles", true);
    xhttp.send(); 
}