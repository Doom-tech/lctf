document.addEventListener('DOMContentLoaded', function() {
    const changeImgButton = document.getElementById('change_img');
    const imageOptions = document.getElementById('image-options');
    const imageInputs = document.querySelectorAll('.image-previews img');
    const saveImageButton = document.getElementById('save-image');
    const cancelImageButton = document.getElementById('cancel-image');
    const imageForm = document.getElementById('image-form');
    const imageInput = document.getElementById('image-input');
    const profileImage = document.getElementById('profile_image');


    changeImgButton.addEventListener('click', function() {
        imageOptions.style.display = 'block'; 
    });

    imageInputs.forEach(img => {
        img.addEventListener('click', function() {
            imageInput.value = this.src.split('/').pop().split('.')[0];  
            profileImage.src = this.src; 
        });
    });

    saveImageButton.addEventListener('click', function(event) {
        event.preventDefault();
 
        fetch('/update_avatar', {
            method: 'POST',
            body: new FormData(imageForm)
        })
        .then(response => {
            if (response.ok) {
                location.reload();
            }
        })
    });
 
    cancelImageButton.addEventListener('click', function() {
        imageOptions.style.display = 'none'; 
    });
});