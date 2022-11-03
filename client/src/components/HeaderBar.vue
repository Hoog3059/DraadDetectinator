<script>
export default {
    props: ['disconnected'],
    data() {
        return {
        }
    }
}
</script>

<template>
    <nav class="navbar" role="navigation" aria-label="main navigation">
        <div class="navbar-brand">
            <div class="navbar-item navbar-name">
                Draad Detectinator
            </div>

            <div class="navbar-item">
                <button class="button">
                    <span class="icon">
                        <font-awesome-icon icon="fa-solid fa-pause" />
                    </span>
                </button>
            </div>

            <div class="navbar-item" :class="disconnected ? '' : 'invisible'">
                <span class="icon-text" id="disconnected">
                    <span class="icon">
                        <span class="fa-stack fa-lg">
                            <font-awesome-icon icon="fa-solid fa-server" class="fa-stack-1x"/>
                            <font-awesome-icon icon="fa-solid fa-slash" class="fa-stack-1x"/>
                        </span>
                    </span>
                </span>
                <span>Disconnected</span>
            </div>
        </div>
    </nav>
</template>

<style>
.navbar {
    background-color: #E5EBf1 !important;
}

.navbar-name {
    font-size: 1.5em;
    font-weight: 600;
}

#disconnected {
    padding-right: 10px;
}

.invisible {
    display: none !important;
}
</style>