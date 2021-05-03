
document.addEventListener('DOMContentLoaded', ()=>{
    fetch("/status")
    .then(resp => resp.json())
    .then(resp => {
        couleur.value = resp.couleur;
        intensite.value = resp.value;
        effet.value = resp.effet;
        on_off(resp.state);
    })
    .catch(err => console.log(err));
});



// recuperation des elements

let couleur = document.getElementById('couleur');
let intensite = document.getElementById('intensite');
let effet = document.getElementById('effet');
const onBtn = document.getElementById('on');
const offBtn = document.getElementById('off');


// a l ecoute de changements

//---------------------------------------------  On
onBtn.addEventListener('click', (e)=>{
    e.preventDefault();
    console.log('click on');
    fetch('/ledOn').then(resp => resp.text()).then(resp => {
        on_off(resp)
    }).catch(err => console.log(err));
});

//-----------------------------------------------  off
offBtn.addEventListener('click', (e)=>{
    e.preventDefault();
    console.log('click off');
    fetch('/ledOff').then(resp => resp.text()).then(resp => {
        on_off(resp);
    }).catch(err => console.log(err));
});


//----------------------------------------------- COULEUR
couleur.addEventListener('change', function(e){
    console.log(e.target.value);

    const val = e.target.value.substring(1, e.target.value.length);
    const addresse = '/LED?couleur=%23' + val;
    fetch(addresse).then(resp => resp.text()).then(resp =>{
        on_off(resp);
    }).catch(err => console.log(err));

});

//------------------------------------------------- INTENSITE
intensite.addEventListener('change', function(e){
    console.log(e.target.value);

    fetch('/LED?intensite=' + e.target.value)
    .then(resp => resp.text()).then(resp =>{
        on_off(resp);
    })
    .catch(err => console.log(err));
});


//-------------------------------------------- EFFETS ------------
effet.addEventListener('change', function(e){
    console.log(e.target.value);

    if(e.target.value != 'Aucun'){
        couleur.disabled = true;

        const box = document.getElementById('box_coul').style.backgroundColor = 'grey';
    }
    else{
        couleur.disabled = false;
        const box = document.getElementById('box_coul').style.backgroundColor = '#eeeeee';
    }


    fetch('/LED?effet=' + e.target.value)
    .then(resp => resp.text()).then(resp =>{
        on_off(resp);
    })
    .catch(err => console.log(err));
});



function on_off(etat){
    if (etat == 'on'){
        onBtn.style.backgroundColor = '#90ee90';
        offBtn.style.backgroundColor = 'initial';
    }
    else{
        onBtn.style.backgroundColor = 'initial';
        offBtn.style.backgroundColor = '#ffcccb';
    }

}