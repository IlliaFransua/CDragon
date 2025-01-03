function showSection(sectionId) {
  const sections = document.querySelectorAll(".section");
  sections.forEach((section) => {
    section.classList.remove("active");
  });

  const selectedSection = document.getElementById(`section-${sectionId}`);
  selectedSection.classList.add("active");
}

function removeSpaces(input) {
  input.value = input.value.replace(/\s+/g, "");
}

function addCategory() {
  const categoryInput = document.getElementById("new-category");
  const categoryText = categoryInput.value.trim();
  const categoryFilter = document.getElementById("category-filter");

  if (categoryText) {
    if (
      !Array.from(categoryFilter.options).some(
        (option) => option.value === categoryText
      )
    ) {
      const newOption = document.createElement("option");
      newOption.value = categoryText;
      newOption.text = categoryText;
      newOption.selected = true;
      categoryFilter.add(newOption);

      categoryInput.value = "";
    } else {
      alert("Такая категория уже существует");
    }
  } else {
    alert("Введите название категории");
  }
}

function addTag() {
  const tagInput = document.getElementById("new-tag");
  const tagText = tagInput.value.trim();
  const tagFilter = document.getElementById("tag-filter");

  if (tagText) {
    if (
      !Array.from(tagFilter.options).some((option) => option.value === tagText)
    ) {
      const newOption = document.createElement("option");
      newOption.value = tagText;
      newOption.text = tagText;
      newOption.selected = true;
      tagFilter.add(newOption);

      tagInput.value = "";
    } else {
      alert("Такой тег уже существует");
    }
  } else {
    alert("Введите название тега");
  }
}
