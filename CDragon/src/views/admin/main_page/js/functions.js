function showSection(sectionId) {
  const sections = document.querySelectorAll(".section");
  sections.forEach((section) => {
    section.classList.remove("active");
  });

  const selectedSection = document.getElementById(`section-${sectionId}`);
  selectedSection.classList.add("active");
}

function redirectToArticleEditor(button) {
  let articleId = button.getAttribute("data-article-id");
  let url = `/admin/article_editor_page/${articleId}`;
  window.location.href = url;
}

function publishArticle(button) {
  let articleId = button.getAttribute("data-article-id");

  if (confirm("Вы уверены, что хотите опубликовать эту статью?")) {
    let url = `/admin/article_editor_page/publish/${articleId}`;

    fetch(url, {
      method: "POST",
    })
      .then((response) => {
        if (response.ok) {
          alert("Статья успешно опубликована");
        } else {
          alert("Произошла ошибка на сервере при публикации статьи");
        }
      })
      .catch((error) => {
        alert("Произошла ошибка при отправке запроса");
      });
  }
}

function deleteArticle(button) {
  let articleId = button.getAttribute("data-article-id");

  if (confirm("Вы уверены, что хотите удалить эту статью?")) {
    let url = `/admin/article_editor_page/delete/${articleId}`;

    fetch(url, {
      method: "POST",
    })
      .then((response) => {
        if (response.ok) {
          let articleCard = button.closest(".sp-article-card");
          if (articleCard) {
            articleCard.remove();
          }
          alert("Статья удалена успешно");
        } else {
          alert("Произошла ошибка на сервере при удалении статьи");
        }
      })
      .catch((error) => {
        alert("Произошла ошибка при отправке запроса");
      });
  }
}

function archiveArticle(button) {
  let articleId = button.getAttribute("data-article-id");

  if (confirm("Вы уверены, что хотите архивировать эту статью?")) {
    let url = `/admin/article_editor_page/archive/${articleId}`;

    fetch(url, {
      method: "POST",
    })
      .then((response) => {
        if (response.ok) {
          alert("Статья удалена архивирована");
        } else {
          alert("Произошла ошибка на сервере при архивировании статьи");
        }
      })
      .catch((error) => {
        alert("Произошла ошибка при отправке запроса");
      });
  }
}
