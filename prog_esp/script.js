
// recuperation des elements

let couleur = document.getElementById('couleur');
let intensite = document.getElementById('intensite');
let effet = document.getElementById('effet');



// a l ecoute de changements

couleur.addEventListener('change', function(e){
    console.log(e.target.value);
});

intensite.addEventListener('change', function(e){
    console.log(e.target.value);
});

effet.addEventListener('change', function(e){
    console.log(e.target.value);
});