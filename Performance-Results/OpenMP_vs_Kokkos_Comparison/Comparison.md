# Side by Side Implementation 
Formula: $$\int_0^1 \frac{4}{1+x^2}\,dx=\pi$$


 ***left***: OpenMP  ***right***: Kokkos 

![side_by_side](https://user-images.githubusercontent.com/38857089/186280291-f5e50c84-a646-4ad3-8385-f190ab858807.png)

# OpenMP Time, Problem Size: $$1^8$$

### Average: 0.041 seconds 

![OpenMP_Time](https://user-images.githubusercontent.com/38857089/186281846-5ea84cb5-e928-43ab-8ef9-cab964f8df50.png)


# Kokkos Time, Problem Size: $$1^8$$

### Average: 0.016 seconds 

![Kokkos_Time](https://user-images.githubusercontent.com/38857089/186281864-1a9a6c8d-6046-42bd-a8c9-c202ab659731.png)


