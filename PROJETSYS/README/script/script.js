var hum = document.getElementById("hamburger");
var submenu = document.getElementById("submenu");
var nav = document.getElementById("nav")
var return_begin = document.getElementById("return_begin");

var bol = false;


hum.addEventListener('click', event =>{
    if(bol === false){
        hum.style.transform = 'rotate(90deg)';
        console.log("yes");
        submenu.style.display = 'block';
        bol = true;
    }else{
        hum.style.transform = 'rotate(0deg)';
        console.log("no");
        submenu.style.display = 'none';
        bol = false;
    }
});

document.addEventListener("scroll", () => {
    if (window.scrollY > 1000) {
        return_begin.style.display = 'block';
    }else{
        return_begin.style.display = 'none';
    }
});


