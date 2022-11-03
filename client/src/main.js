import { createApp } from 'vue/dist/vue.esm-bundler'
import App from './App.vue'

/* import the fontawesome core */
import { library } from '@fortawesome/fontawesome-svg-core'

/* import font awesome icon component */
import { FontAwesomeIcon } from '@fortawesome/vue-fontawesome'

/* import specific icons */
import { faPause, faPlay, faServer, faSlash } from '@fortawesome/free-solid-svg-icons'

/* add icons to the library */
library.add(faPause)
library.add(faPlay)
library.add(faServer)
library.add(faSlash)

createApp(App).component('font-awesome-icon', FontAwesomeIcon).mount('#app')
